/* Most of this is stolen from iloader: ttps://xerub.github.io/ios/iboot/2018/05/10/de-rebus-antiquis.html */

static inline uint16_t be16(uint16_t x) { return __builtin_bswap16(x); }
static inline uint32_t be32(uint32_t x) { return __builtin_bswap32(x); }
static inline uint64_t be64(uint64_t x) { return __builtin_bswap64(x); }

#define R0 (0)
#define R1 (1)
#define R2 (2)
#define R3 (3)
#define R4 (4)
#define R5 (5)
#define R6 (6)
#define R7 (7)
#define R8 (8)
#define R9 (9)
#define SB (R9)
#define R10 (10)
#define SL (R10)
#define R11 (11)
#define FP (R11)
#define R12 (12)
#define IP (R12)
#define R13 (13)
#define SP (R13)
#define R14 (14)
#define LR (R14)
#define R15 (15)
#define PC (R15)
#define R0_R7 (0xFF)

#define INSNW_LDR_W_RT_OFF(d, off) be32(0xDDF80000 | be16((((d) & 0xF) << 12) | ((off & 0xFFF))))
#define INSNW_STR_W_RT_OFF(d, off) be32(0xCDF80000 | be16((((d) & 0xF) << 12) | ((off & 0xFFF))))
#define INSNW_MOVW_SP_0            (uint32_t)0x0D00F240
#define INSNW_MOVT_SP_4010         (uint32_t)0x0D10F2C4
#define INSNW_MOVT_SP_8010         (uint32_t)0x0D10F2C8
#define INSN2_BX_R12__NOP          (uint32_t)0xBF004760
#define INSN2_LDR_R1_PC__B_PLUS4   (uint32_t)0xE0014900
#define INSN2_MOV_R0_0__STR_R0_R3  (uint32_t)0x60182000
#define INSN2_MOV_R0_1__MOV_R0_1   (uint32_t)0x20012001
#define INSN2_NOP__NOP			   (uint32_t)0xBF00BF00
#define INSN2_RETURN_0			   (uint32_t)0x47702000
#define INSN2_RETURN_21			   (uint32_t)0x47702021
#define INSNT_BLX_R(d)			   (uint16_t)(INSNT_BX_R(d) | 0x80)
#define INSNT_BX_R(d)			   (uint16_t)(0x4700 | (((d) & 15) << 3))
#define INSNT_B_PC4			       (uint16_t)0xE002
#define INSNT_LDR_R_PC(d, n)       (uint16_t)(0x4800 | (((d) & 7) << 8) | ((n) / 4))
#define INSNT_ADD_R_I(d, i)        (uint16_t)(0x3000 | (((d) & 15) << 8) | ((i) & 0xFF))
#define INSNT_SUB_R_I(d, i)        (uint16_t)(0x3C00 | (((d) & 15) << 8) | ((i) & 0xFF))
#define INSNT_ADD_SP_I(i)          (uint16_t)(0xB000 | ((i >> 2) & 0x7F))
#define INSNT_SUB_SP_I(i)          (uint16_t)(0xB080 | ((i >> 2) & 0x7F))
#define INSNT_MOV_R_I(d, i)		   (uint16_t)(0x2000 | (((d) & 7) << 8) | ((i) & 0xFF))
#define INSNT_MOV_R_R(d, s)		   (uint16_t)(0x4600 | (((s) & 15) << 3) | ((d) & 7) | (((d) & 8) << 4))
#define INSNT_PUSH_R(regs)         (uint16_t)(0xB400 | (regs) & 0xFF)
#define INSNT_POP_R(regs)          (uint16_t)(0xBC00 | (regs) & 0xFF)

#define INSNT_NOP			       (uint16_t)0xBF00
#define INSNT_POP_PC			   (uint16_t)0xBD00
#define INSNT_POP_R0_PC			   (uint16_t)0xBD01
#define INSNT_PUSH_R0			   (uint16_t)0xB401
#define INSNT_PUSH_R0_R1		   (uint16_t)0xB403
#define INSNT_RETURN			   (uint16_t)0x4770
#define INSNT_STR_R0_SP4		   (uint16_t)0x9001
#define INSNT_STR_R1_R0_68		   (uint16_t)0x6441
#define INSNT_STR_R1_R4_R0		   (uint16_t)0x5021
#define INSNW_LDR_R12_PC4		   (uint16_t)0xC004F8DF
#define INSNW_LDR_SP_PC72		   (uint16_t)0xD048F8DF
#define INSNW_LDR_SP_PC80		   (uint16_t)0xD050F8DF
#define INSNW_MOV_R1_2400		   (uint16_t)0x5110F44F
#define INSNW_MOV_R1_40000000      (uint32_t)0x4180F04F
#define INSNW_MOV_R1_80000000      (uint32_t)0x4100F04F
#define INSNW_STRH_R1_R4_E2C       (uint32_t)0x1E2CF8A4
#define INSNW_STRH_R1_R4_E54       (uint32_t)0x1E54F8A4

#define INSNA_BX_R0			       (uint32_t)0xE12FFF10
#define INSNA_LDR_PC_NEXT	       (uint32_t)0xE51FF004
#define INSNA_MOV_R1_0		       (uint32_t)0xE3A01000
#define INSNA_MOV_R2_0		       (uint32_t)0xE3A02000
#define INSNA_MOV_R2_40000000	   (uint32_t)0xE3A02101
#define INSNA_NOP			       (uint32_t)0xE1A00000
#define INSNA_RETURN			   (uint32_t)0xE12FFF1E

#define INSNT_ILLEGAL			   (uint16_t)0xdef0
#define INSNA_ILLEGAL			   (uint32_t)0xe7ffdef0

static inline void write_insn_16(uint8_t* buf, size_t* index, uint16_t insn) {
    *(uint16_t*)&buf[*index] = insn;
    *index += sizeof(uint16_t);
}

static inline void write_insn_32(uint8_t* buf, size_t* index, uint32_t insn) {
    *(uint32_t*)&buf[*index] = insn;
    *index += sizeof(uint32_t);
}

#define WRITE_INSN(buf, i, insn) _Generic(insn, uint16_t:write_insn_16, uint32_t:write_insn_32)(buf, &i, insn)

static inline uint32_t make_bl(int blx, int pos, int tgt) {
    int delta;
    unsigned short pfx;
    unsigned short sfx;

    unsigned int omask = 0xF800;
    unsigned int amask = 0x7FF;

    if (blx) { /* XXX untested */
        omask = 0xE800;
        amask = 0x7FE;
        pos &= ~3;
    }

    delta = tgt - pos - 4; /* range: 0x400000 */
    pfx = 0xF000 | ((delta >> 12) & 0x7FF);
    sfx =  omask | ((delta >>  1) & amask);

    return (unsigned int)pfx | ((unsigned int)sfx << 16);
}
