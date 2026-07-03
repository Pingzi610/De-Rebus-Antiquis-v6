#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/endian.h>
#include <asm.h>
#define DEBUG_IBOOT_BUILDER
#include <target.h>

uint8_t* buf;
size_t file_len;
size_t spare_memory_off = 0;

uint8_t readp_bkpt_handler[] = {
    #embed "breakpoint_handlers/readp_handler.bin"
};

uint8_t memalign_start_bkpt_handler[] = {
    #embed "breakpoint_handlers/memalign_start_handler.bin"
};

uint8_t memalign_loop_bkpt_handler[] = {
    #embed "breakpoint_handlers/memalign_loop_handler.bin"
};

uint8_t memalign_end_bkpt_handler[] = {
    #embed "breakpoint_handlers/memalign_end_handler.bin"
};

uint8_t generic_bkpt_handler[] = {
    #embed "breakpoint_handlers/generic_handler.bin"
};

uint8_t stack_dump_handler[] = {
    #embed "breakpoint_handlers/stack_dump_handler.bin"
};

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

void add_breakpoint(size_t bkpt_address, void* bkpt_handler, size_t bkpt_handler_len, int new_sp) {
    size_t bp_bl_offset = 0;
    size_t bkpt_trampoline_start = spare_memory_off;
    WRITE_INSN(buf, spare_memory_off, INSNT_NOP);
    WRITE_INSN(buf, spare_memory_off, INSNT_MOV_R_R(new_sp, SP));

    WRITE_INSN(buf, spare_memory_off, SP_MOVW);
    WRITE_INSN(buf, spare_memory_off, SP_MOVT);
    WRITE_INSN(buf, spare_memory_off, INSNT_PUSH_R(R0_R7));
    WRITE_INSN(buf, spare_memory_off, INSNT_SUB_SP_I(0x18));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(R8, 0x0));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(R9, 0x4));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(R10, 0x8));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(R11, 0xC));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(R12, 0x10));
    WRITE_INSN(buf, spare_memory_off, INSNW_STR_W_RT_OFF(LR, 0x14));
    bp_bl_offset = spare_memory_off;
    WRITE_INSN(buf, spare_memory_off, (uint32_t)0); /* Placeholder until we copy in our breakpoint handler */
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(R8, 0x0));
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(R9, 0x4));
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(R10, 0x8));
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(R11, 0xC));
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(R12, 0x10));
    WRITE_INSN(buf, spare_memory_off, INSNW_LDR_W_RT_OFF(LR, 0x14));
    WRITE_INSN(buf, spare_memory_off, INSNT_ADD_SP_I(0x18));
    WRITE_INSN(buf, spare_memory_off, INSNT_POP_R(R0_R7));
    WRITE_INSN(buf, spare_memory_off, INSNT_MOV_R_R(SP, new_sp));
    memcpy(&buf[spare_memory_off], &buf[bkpt_address], sizeof(uint32_t));
    spare_memory_off += sizeof(uint32_t);
    WRITE_INSN(buf, spare_memory_off, INSNT_BX_R(LR));

    /* Branch to the BKPT trampoline */
    *(uint32_t*)&buf[bkpt_address] = make_bl(0, bkpt_address, bkpt_trampoline_start | 1);

    /* Branch to the actual breakpoint code */
    *(uint32_t*)&buf[bp_bl_offset] = make_bl(0, bp_bl_offset, spare_memory_off | 1);

    memcpy(&buf[spare_memory_off], bkpt_handler, bkpt_handler_len);
    spare_memory_off += bkpt_handler_len;

    /* Get the next four byte aligned address */
    spare_memory_off = (spare_memory_off + 3) & ~3;
}

void apply_patches() {
    /* Find the spare memory offset. */
    uint8_t* apple_cert = memmem(buf, file_len, "Apple Certification Authority", strlen("Apple Certification Authority"));
    spare_memory_off = apple_cert - buf;

    /* Get the next four byte aligned address */
    spare_memory_off = (spare_memory_off + 3) & ~3;

    /* Rename some environment variables so we don't break standard booting */
    char* boot_ramdisk = memmem(buf, file_len, "boot-ramdisk", sizeof("boot-ramdisk"));
    strcpy(boot_ramdisk, "noot-ramdisk");
    char* boot_partition = memmem(buf, file_len, "boot-partition", sizeof("boot-partition"));
    strcpy(boot_partition, "noot-partition");

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <iboot_in> <iboot_out>\n", argv[0]);
        return 1;
    }
    read_file_into_buffer(argv[1], &buf, &file_len);
    apply_patches();
    add_breakpoint(READP_BP_OFF, readp_bkpt_handler, sizeof(readp_bkpt_handler), READP_SAFE_REG);
    add_breakpoint(MEMALIGN_START_BP_OFF, memalign_start_bkpt_handler, sizeof(memalign_start_bkpt_handler), MEMALIGN_START_SAFE_REG);
    add_breakpoint(MEMALIGN_LOOP_BP_OFF, memalign_loop_bkpt_handler, sizeof(memalign_loop_bkpt_handler), MEMALIGN_LOOP_SAFE_REG);
    add_breakpoint(MEMALIGN_END_BP_OFF, memalign_end_bkpt_handler, sizeof(memalign_end_bkpt_handler), MEMALIGN_END_SAFE_REG);

    write_file_from_buffer(argv[2], &buf, file_len);

    return 0;
}