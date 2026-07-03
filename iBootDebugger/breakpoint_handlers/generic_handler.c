#include <stdint.h>
#include <target.h>

void _start() {
    register uint32_t* reg_r0 __asm("r0");
    uint32_t* r0 = reg_r0;
    register uint32_t* reg_r1 __asm("r1");
    uint32_t* r1 = reg_r1;
    register uint32_t* reg_r2 __asm("r2");
    uint32_t* r2 = reg_r2;
    register uint32_t* reg_r3 __asm("r3");
    uint32_t* r3 = reg_r3;
    register uint32_t* reg_r4 __asm("r4");
    uint32_t* r4 = reg_r4;
    register uint32_t* reg_r5 __asm("r5");
    uint32_t* r5 = reg_r5;
    register uint32_t* reg_r6 __asm("r6");
    uint32_t* r6 = reg_r6;
    register uint32_t* reg_r7 __asm("r7");
    uint32_t* r7 = reg_r7;
    register uint32_t* reg_r8 __asm("r8");
    uint32_t* r8 = reg_r8;
    register uint32_t* reg_r9 __asm("r9");
    uint32_t* r9 = reg_r9;
    register uint32_t* reg_r10 __asm("r10");
    uint32_t* r10 = reg_r10;
    register uint32_t* reg_r11 __asm("r11");
    uint32_t* r11 = reg_r11;
    register uint32_t* reg_r12 __asm("r12");
    uint32_t* r12 = reg_r12;
    register uint32_t* reg_sp __asm("sp");
    uint32_t* sp = reg_sp;
    register uint32_t* reg_lr __asm("lr");
    uint32_t* lr = reg_lr;

    printf("Breakpoint hit! r0=%p, r1=%p, r2=%p, r3=%p, r4=%p, r5=%p, r6=%p, r7=%p, r8=%p, r9=%p, r10=%p, r11=%p, r12=%p, sp=%p, lr=%p\n", r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr);
}