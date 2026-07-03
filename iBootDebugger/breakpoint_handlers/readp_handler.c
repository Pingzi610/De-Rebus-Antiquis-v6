#include <stdint.h>
#include <target.h>

void _start(void *ih, void *buffer, long offset) {
    register unsigned reg_sp __asm(READP_SP_REG);
    uint32_t sp = reg_sp;
    uint32_t length = *(uint32_t*)(sp + 0x10);

    printf("readp(%p, %p, 0x%x, %d)\n", ih, buffer, offset, length);
}