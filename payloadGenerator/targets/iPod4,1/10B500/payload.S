.thumb
.syntax unified
.text

.global _start
.code 16

_start:
    @ Reset the SP.
    ldr.w sp, =SAFE_SP

    @ Disable interrupts.
    ldr r3, =DISABLE_INTERRUPTS
    blx r3

    @ Patch Diags handler type 5 (diags) -> 0 (unknown)
    mov r0, #0x94C
    ldr r1, =0x21004831
    str r1, [r0]

    @ Patch mov r0, 'diag' -> mov r0, 'ibox'
    ldr r1, =0x7078F646
    str r1, [r0, #0x8]
    ldr r1, =0x1062F6C6
    str r1, [r0, #0xC]

    @ Disable signature checks
    ldr r0, =0x183F0
    ldr r1, =0x60182000
    str r1, [r0]

    @ Load the image at 0x44000000 rather than 0x40000000
    @ (seems to be a requirement for iBoots < iOS6).
    mov r0, #0x910
    ldr r1, =0x4088f04f
    str r1, [r0]

    @ Patch fsboot handler -> diags handler
    ldr r0, =0x42004
    mov r1, #0x941
    str r1, [r0]

    @ Run nettoyeur
    ldr r0, =NETTOYEUR
    blx r0

    @ Disable the cache.
    ldr r7, =ARM_CPU_QUIESCE
    blx r7
    
    @ Restart iBoot.
    ldr r7, =IBOOT_ADDR
    bx r7

.align 4
.space 0x10 @ This space needs to == 0
.pool
.set SAFE_SP,                0x5fff8000
.set DISABLE_INTERRUPTS,     0x5ff33b19
.set ARM_CPU_QUIESCE,        0x5ff1eb81
.set NETTOYEUR,              0x5FF459ED
.set IBOOT_ADDR,             0x5ff00000
