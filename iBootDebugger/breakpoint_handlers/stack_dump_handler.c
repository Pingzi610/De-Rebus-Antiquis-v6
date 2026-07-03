#include <stdint.h>
#include <stddef.h>

#include <target.h>

void _start() {
    register uint32_t reg_sp __asm(STACK_DUMP_SP_REG);
    uint32_t sp = reg_sp;

    if(sp > SP_NEAR_BIN_LIST)
        return;

    fix_printf();
    printf("STACK DUMP! sp=%p\n", sp);

    for(int32_t i = -0x10; i <= 0x10; i += 0x4) {
        uint32_t* off = (uint32_t*)(sp + i);
        printf("%p: %p", off, *off);
        off = (uint32_t*)*off;
        for(int j = 0; j < 5; j += 1) {
            int is_addr = (((uint32_t)off) & 0xFFF00000) == IBOOT_BASE_ADDR;
            if(!is_addr)
                break;
            printf(" (%p)", *off);
            off = (uint32_t*)*off;
        }
        printf("\n");
    }
}