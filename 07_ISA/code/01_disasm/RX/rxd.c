//
// Copyright VINJEY Software Systems Pvt. Ltd, All Rights Reserved.
//
// All rights reserved. Any attempt or participation in deciphering, decoding,
// reverse engineering or in any way altering the source code is strictly
// prohibited, unless the prior written consent of VINJEY is obtained.
//
// Filename         :  rxd.c
// Description      :  Renesas RX disassembler code
// Version Number   :  1.0
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

//
// Compile Time Macros
//

// Maximum length for opcode/operands
#define RX_MAX_OP_LEN                                         64

// Maximum number of opcode + operands
#define RX_MAX_OPC_OP_CNT                                     16

//
// Enumerations
//

// Zero Operand Instructions
typedef enum __rxd_zero_op_igrp
{
    RX_GRP_ZERO_OP1,                                          // Opcode with zero operands
    RX_GRP_ZERO_OP2                                           // Opcode with zero operands
} RXD_ZeroOpGrp;

// Integer ALU Shift Groups
typedef enum __rxd_sh_igrp
{
    RX_GRP_IA_SH_REG1,                                        // <opc> Rs, Rd
    RX_GRP_IA_SH_REG2,                                        // <opc> Rs, Rs2, Rd
    RX_GRP_IA_SH_IMM1,                                        // <opc> #sh, Rd (2 Byte encoding)
    RX_GRP_IA_SH_IMM2,                                        // <opc> #sh, Rd (3 Byte encoding)
    RX_GRP_IA_SH_CRY                                          // <opc> Rd
} RXD_ShiftGrp;

// Integer ALU Instructions with Immediate and Register Operand
typedef enum __rxd_ia_ir_grp
{
    RX_GRP_IA_AB_IR1,                                         // <opc> #uimm4, reg
    RX_GRP_IA_AB_IR2C,                                        // <opc> #uimm8, reg
    RX_GRP_IA_AB_IR3,                                         // <opc> #imm, reg (2 byte opcode)
    RX_GRP_IA_AB_IR4,                                         // <opc> #imm, reg (3 byte opcode)
    RX_GRP_IA_AB_IR5                                          // <opc> #imm, reg, reg
} RXD_IA_IR_Grp;

// Integer ALU Instructions with Register Operands only
typedef enum __rxd_ia_reg_grp
{
    RX_GRP_IA_AB_REG1,                                        // <opc> Rd
    RX_GRP_IA_AB_REG2                                         // <opc> Rs, Rs2, Rd
} RXD_IA_REG_Grp;

// Integer ALU Instructions with Register/Memory source and Register destination
typedef enum __rxd_ia_rmem_grp
{
    RX_GRP_IA_AB_LD1,                                         // <opc> Rs/[Rs].UB/dsp[Rs].UB, Rd (2 byte opcode)
    RX_GRP_IA_AB_LD2,                                         // <opc> Rs/[Rs].UB/dsp[Rs].UB, Rd (3 byte opcode)
    RX_GRP_IA_AB_MI1,                                         // <opc> [Rs].mi/dsp[Rs].mi, Rd (3 byte opcode)
    RX_GRP_IA_AB_MI2                                          // <opc> [Rs].mi/dsp[Rs].mi, Rd (4 byte opcode)
} RXD_IA_RMEM_Grp;

// Load/Store Instruction groups
typedef enum __rxd_ls_grp
{
    RX_GRP_ST_SOFF,                                           // Store small offset, MOV.sz Rs, dsp5[Rd]
    RX_GRP_LD_SOFF_S,                                         // Load small offset(signed), MOV.sz dsp5[Rs], Rd
    RX_GRP_LD_SOFF_U,                                         // Load small offset(unsigned), MOVU.sz dsp5[Rs], Rd
    RX_GRP_ST_OFF,                                            // Store offset, MOV.sz Rs, [Rd]/dsp[Rd]
    RX_GRP_LD_OFF_S,                                          // Load offset(signed), MOV.sz [Rs]/dsp[Rs], Rd
    RX_GRP_LD_OFF_U,                                          // Load offset(unsigned), MOVU.sz Rs/[Rs]/dsp[Rs], Rd
    RX_GRP_ST_IDX,                                            // Store Base + Index, MOV.sz Rs, [Ri, Rb]
    RX_GRP_LD_IDX_S,                                          // Load Base + Index(Signed), MOV.sz [Ri, Rb], Rd
    RX_GRP_LD_IDX_U,                                          // Load Base + Index(Unsigned), MOVU.sz [Ri, Rb], Rd
    RX_GRP_ST_SEQ_S,                                          // Store Sequential, MOV.sz Rs, [Rd+]/[-Rd]
    RX_GRP_LD_SEQ_S,                                          // Load(Unsigned) Sequential, MOV.sz [Rs+]/[-Rs], Rd
    RX_GRP_LD_SEQ_U                                           // Load(Unsigned) Sequential, MOVU.sz [Rs+]/[-Rs], Rd
} RXD_LdStGrp;

// Stack Instruction groups
typedef enum __rxd_stk_grp
{
    RX_GRP_STK_MI,                                            // Push multiple, PUSHM Rs-Rs2
    RX_GRP_STK_MO,                                            // Pop multiple, POPM Rd-Rd2
    RX_GRP_STK_SI,                                            // Push register, PUSH.sz Rs
    RX_GRP_STK_SO,                                            // Pop register, POP Rd
    RX_GRP_STK_CTL,                                           // Push/Pop control register, PUSHC/POPC Cr
    RX_GRP_STK_SIM                                            // Push memory, PUSH.sz [Rs]/dsp[Rs]
} RXD_StkGrp;

// Data Movement instructions apart from load/store and stack
typedef enum __rxd_dm_grp
{
    RX_GRP_DM_IR,                                             // MOV.L #uimm8, Rd
    RX_GRP_DM_IRS,                                            // MOV.L #imm, Rd
    RX_GRP_DM_RR,                                             // MOV.sz Rs, Rd
    RX_GRP_DM_IMS,                                            // MOV.sz #uimm8, dsp5[Rd]
    RX_GRP_DM_IM,                                             // MOV.sz #imm, [Rd]/dsp[Rd]
    RX_GRP_DM_MM,                                             // MOV.sz dsp[Rs], dsp[Rd]
    RX_GRP_DM_CMOV,                                           // STZ/STNZ Rs, Rd
    RX_GRP_DM_CRES,                                           // SCCnd.L Rd/SCCnd.sz [Rd]/dsp[Rd]
    RX_GRP_DM_EX_LD,                                          // MOVLI [Rs], Rd
    RX_GRP_DM_EX_ST,                                          // MOVCO Rs, [Rd]
    RX_GRP_DM_XCHG_LD,                                        // XCHG Rs/[Rs].UB/dsp[Rs].UB, Rd
    RX_GRP_DM_XCHG_MI                                         // XCHG [Rs].mi/dsp[Rs].mi, Rd
} RXD_DataMvGrp;

// Array Operation Groups
typedef enum __rxd_ary_igrp
{
    RX_GRP_ARRAY_SZ,                                          // Opcodes with sz part of it
    RX_GRP_ARRAY_BYTE                                         // Opcodes without sz being part of it and byte
} RXD_ArrayGrp;

// System Groups
typedef enum __rxd_sys_igrp
{
    RX_GRP_SYS_MTC,                                           // Move to Control Register
    RX_GRP_SYS_MFC,                                           // Move from Control Register
    RX_GRP_SYS_MTCI,                                          // Move to Control Register Immediate
    RX_GRP_SYS_IPL,                                           // Move to Interrupt Priority Level
    RX_GRP_SYS_INT,                                           // Interrupt Generation
    RX_GRP_SYS_PSW,                                           // Set/Clear flags in PSW
    RX_GRP_SYS_RBR,                                           // Register Bank with Bank index in register
    RX_GRP_SYS_RBI                                            // Register Bank with Bank index in immediate
} RXD_SysGrp;


// DSP Instructions
typedef enum __rxd_dsp_igrp
{
    RX_GRP_DSP_MUL,                                           // <opc> Rs, Rs2 [, A]
    RX_GRP_DSP_TAC,                                           // <opc> Rs [, A]
    RX_GRP_DSP_RND,                                           // <opc> #imm1 [, A]
    RX_GRP_DSP_FAC                                            // <opc> [#imm2, A, ] Rd
} RXD_DspGrp;

// Bit Manipulation Instruction groups
typedef enum __rxd_bm_grp
{
    RX_GRP_BM_IR1,                                            // <opc> #imm5, Rd
    RX_GRP_BM_IR1C,                                           // <opc> #imm5, Rs
    RX_GRP_BM_IR2,                                            // BMCnd #imm5, Rd
    RX_GRP_BM_IR3,                                            // BNOT #imm5, Rd
    RX_GRP_BM_IM1,                                            // <opc> #imm3, [Rd/Rs].B/dsp[Rd/Rs].B
    RX_GRP_BM_IM1C,                                           // <opc> #imm3, [Rs].B/dsp[Rs].B
    RX_GRP_BM_IM2,                                            // BMCnd #imm3, [Rd].B/dsp[Rd].B
    RX_GRP_BM_IM3,                                            // BNOT #imm3, [Rd].B/dsp[Rd].B
    RX_GRP_BM_REG,                                            // BSET/BCLR/BTST/BNOT Rs, Rd/[Rd].B/dsp[Rd].B
    RX_GRP_BM_RNG                                             // BFMOVZ/BFMOV #slsb, #dlsb, #width, Rs, Rd (v3)
} RXD_BMGrp;

// Control Transfer Instruction groups
typedef enum __rxd_ctl_grp
{
    RX_GRP_CTRL_IMM_S,                                        // BRA.S/BEQ.S/BNE.S pcdsp:3
    RX_GRP_CTRL_IMM_B,                                        // BCnd.B/BRA.B pcdsp:8
    RX_GRP_CTRL_IMM_W,                                        // BRA.W/BSR.W/BEQ.W/BNE.W pcdsp:16
    RX_GRP_CTRL_IMM_A,                                        // BRA.A/BSR.A pcdsp:24
    RX_GRP_CTRL_REG,                                          // JMP/JSR/BRA.L/BSR.L Rs
    RX_GRP_CTRL_ACA,                                          // RTSD #uimm
    RX_GRP_CTRL_ACR                                           // RTSD #uimm, Rd-Rd2
} RXD_CtrlGrp;

// Floating Point Instruction groups
typedef enum __rxd_fpu_grp
{
    RX_GRP_FPU_I2F_LD,                                        // ITOF/UTOF Rs/[Rs].UB/dsp[Rs].UB, Rd
    RX_GRP_FPU_I2F_MI,                                        // ITOF/UTOF [Rs].mi/dsp[Rs].mi, Rd
    RX_GRP_FPU_LD,                                            // <opc> Rs/[Rs].L/dsp[Rs].L, Rd
    RX_GRP_FPU_IMM,                                           // <opc> #imm32, Rd
    RX_GRP_FPU_3REG                                           // FADD/FSUB/FMUL Rs, Rs2, Rd (v2+)
} RXD_FpuGrp;

// Double Precision Instruction groups
typedef enum __rxd_dpu_grp
{
    RX_GRP_DPU_RTLI,                                          // DMOV.L #imm32, DRLd
    RX_GRP_DPU_RTHI,                                          // DMOV.L/DMOV.D #imm32, DRHd
    RX_GRP_DPU_ST,                                            // DMOV.D DRs, [Rd]/dsp[Rd]
    RX_GRP_DPU_LD,                                            // DMOV.D [Rs]/dsp[Rs], DRd
    RX_GRP_DPU_MVF,                                           // MVFDR
    RX_GRP_DPU_BA,                                            // DADD/DSUB/DMUL/DDIV DRs, DRs2, DRd
    RX_GRP_DPU_CMP,                                           // DCMPcm DRs, DRs2
    RX_GRP_DPU_UAM,                                           // DMOV.D/DABS/DNEG/DSQRT DRs, DRd
    RX_GRP_DPU_CFD,                                           // DTOI/DTOU/DTOF/DROUND DRs, DRd
    RX_GRP_DPU_PUSHC,                                         // DPUSHM.L DCRs-DCRs2
    RX_GRP_DPU_POPC,                                          // DPOPM.L DCRd-DCRd2
    RX_GRP_DPU_PUSH,                                          // DPUSHM.D DRs-DRs2
    RX_GRP_DPU_POP,                                           // DPOPM.D DRd-DRd2
    RX_GRP_DPU_RFL,                                           // DMOV.L DRLs, Rd
    RX_GRP_DPU_RFH,                                           // DMOV.L DRHs, Rd
    RX_GRP_DPU_RFC,                                           // MVFDC DCRs, Rd
    RX_GRP_DPU_RTL,                                           // DMOV.L Rs, DRLd
    RX_GRP_DPU_RTH,                                           // DMOV.L/DMOV.D Rs, DRHd
    RX_GRP_DPU_RTC,                                           // MVTDC Rs, DCRd
    RX_GRP_DPU_CTD                                            // ITOD/FTOD/UTOD Rs, DRd
} RXD_DpuGrp;

//
// Typedefs
//

// Exact width integer data types
typedef int8_t vint8;
typedef int16_t vint16;
typedef int32_t vint32;
typedef int64_t vint64;
typedef __int128 vint128;
typedef uint8_t vuint8;
typedef uint16_t vuint16;
typedef uint32_t vuint32;
typedef uint64_t vuint64;
typedef unsigned long long vdouble64;

typedef int vint;
typedef float vfloat32;
typedef __uint128_t vuint128;
typedef char vchar;

//
// Globals
//

vchar cbuf[RX_MAX_OP_LEN * RX_MAX_OPC_OP_CNT];

//
// Tables
//

// sz field string
vchar *RXD_SZ_str[] = {
    "B",
    "W",
    "L",
    NULL
};

// Memory Identifier String
vchar *RXD_MI_str[] = {
    "B",
    "W",
    "L",
    "UW"
};

// mi field scaling for displacement
vint RXD_MI_scale[] = {
    1,
    2,
    4,
    2
};

// flag operand string
vchar *RXD_flag_str[] = {
    "C",
    "Z",
    "S",
    "O",
    NULL,
    NULL,
    NULL,
    NULL,
    "I",
    "U",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// Control Register Operand String
vchar *RXD_Cr_str[] = {
    "PSW",
    "PC",
    "USP",
    "FPSW",
    NULL,
    NULL,
    NULL,
    NULL,
    "BPSW",
    "BPC",
    "ISP",
    "FINTV",
    "INTB",
    "EXTB",
    NULL,
    NULL
};

// Condition code string (cd)
vchar *RXD_COND_str[] = {
    "EQ",
    "NE",
    "GEU",
    "LTU",
    "GTU",
    "LEU",
    "PZ",
    "N",
    "GE",
    "LT",
    "GT",
    "LE",
    "O",
    "NO",
    NULL,
    NULL
};

//
// Opcode tables
//

// SH.CRY opcodes
vchar *RXD_IALU_SH_CRY_opcodes[] = {
    "RORC",
    "ROLC"
};

// SH.IMM1 opcodes
vchar *RXD_IALU_SH_IMM1_opcodes[] = {
    "SHLR",
    "SHAR",
    "SHLL",
    NULL
};

// SH.IMM2 opcodes
vchar *RXD_IALU_SH_IMM2_opcodes[] = {
    "ROTR",
    "ROTL"
};

// SH.REG1 opcodes
vchar *RXD_IALU_SH_REG1_opcodes[] = {
    "SHLR",
    "SHAR",
    "SHLL",
    NULL,
    "ROTR",
    "REVW",
    "ROTL",
    "REVL"
};

// SH.REG2 opcodes
vchar *RXD_IALU_SH_REG2_opcodes[] = {
    "SHLR",
    "SHAR",
    "SHLL",
    NULL
};

// ARY.B opcodes
vchar *RXD_ARY_B_opcodes[] = {
    "SCMPU",
    "SMOVU",
    "SMOVB",
    "SMOVF"
};

// ARY.SZ opcodes
vchar *RXD_ARY_SZ_opcodes[] = {
    "SUNTIL",
    "SWHILE",
    "SSTR",
    "RMPA"
};

// ZERO.OP1 opcodes
vchar *RXD_ZERO_OP1_opcodes[] = {
    NULL,
    NULL,
    NULL,
    "SATR",
    "RTFI",
    "RTE",
    "WAIT",
    NULL
};

// ZERO.OP2 opcodes
vchar *RXD_ZERO_OP2_opcodes[] = {
    "BRK",
    NULL,
    "RTS",
    "NOP",
};

// SYS.PSW opcodes
vchar *RXD_SYS_PSW_opcodes[] = {
    "SETPSW",
    "CLRPSW"
};

// SYS.RB opcodes
vchar *RXD_RB_opcodes[] = {
    "SAVE",
    "RSTR"
};

// DSP.MUL opcodes (v2 and v3)
vchar *RXD_DSP_MUL_opcodes_v2[] = {
    "MULHI",
    "MULLO",
    "MULLH",
    "EMULA",
    "MACHI",
    "MACLO",
    "MACLH",
    "EMACA",
    NULL,
    NULL,
    NULL,
    NULL,
    "MSBHI",
    "MSBLO",
    "MSBLH",
    "EMSBA"
};

// DSP.MUL opcodes (v1)
vchar *RXD_DSP_MUL_opcodes_v1[] = {
    "MULHI",
    "MULLO",
    NULL,
    NULL,
    "MACHI",
    "MACLO",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// DSP.TAC opcodes (v2 and v3)
vchar *RXD_DSP_TAC_opcodes_v2[] = {
    "MVTACHI",
    "MVTACLO",
    NULL,
    "MVTACGU"
};

// DSP.TAC opcodes (v1)
vchar *RXD_DSP_TAC_opcodes_v1[] = {
    "MVTACHI",
    "MVTACLO",
    NULL,
    NULL
};

// DSP.FAC opcodes (v2 and v3)
vchar *RXD_DSP_FAC_opcodes_v2[] = {
    "MVFACHI",
    "MVFACLO",
    "MVFACMI",
    "MVFACGU"
};

// DSP.FAC opcodes (v1)
vchar *RXD_DSP_FAC_opcodes_v1[] = {
    "MVFACHI",
    NULL,
    "MVFACMI",
    NULL
};

// DSP.RND opcodes (v2 and v3)
vchar *RXD_DSP_RND_opcodes_v2[] = {
    "RACW",
    "RDACW",
    "RACL",
    "RDACL"
};

// DSP.RND opcodes (v1)
vchar *RXD_DSP_RND_opcodes_v1[] = {
    "RACW",
    NULL,
    NULL,
    NULL
};

// AB.IR1(C) opcodes
vchar *RXD_IA_AB_IR1_opcodes[] = {
    "SUB",
    "CMP",
    "ADD",
    "MUL",
    "AND",
    "OR",
    "MOV.L",
    NULL
};
 
// AB.IR3(C) opcodes
vchar *RXD_IA_AB_IR3_opcodes[] = {
    "CMP",
    "MUL",
    "AND",
    "OR"
};
 
// AB.IR4(C) opcodes
vchar *RXD_IA_AB_IR4_opcodes[] = {
    NULL,
    NULL,
    "ADC",
    NULL,
    "MAX",
    "MIN",
    "EMUL",
    "EMULU",
    "DIV",
    "DIVU",
    NULL,
    NULL,
    "TST",
    "XOR",
    "STZ",
    "STNZ"
};

// AB.REG1 opcodes
vchar *RXD_IA_AB_REG1_opcodes[] = {
    "NOT",
    "NEG",
    "ABS",
    "SAT"
};

// AB.REG2 opcodes v1
vchar *RXD_IA_AB_REG2_opcodes_v1[] = {
    "SUB",
    NULL,
    "ADD",
    "MUL",
    "AND",
    "OR",
    NULL,
    NULL
};

// AB.REG2 opcodes
vchar *RXD_IA_AB_REG2_opcodes_v3[] = {
    "SUB",
    NULL,
    "ADD",
    "MUL",
    "AND",
    "OR",
    "XOR",
    NULL
};

// AB.LD1(C)/AB.MI(C) opcodes
vchar *RXD_IA_AB_RMEM1_opcodes[] = {
    "SUB",
    "CMP",
    "ADD",
    "MUL",
    "AND",
    "OR",
    NULL,
    NULL
};
 
// AB.LD2
vchar *RXD_IA_AB_LD2_opcodes[] = {
    "SBB",
    "NEG",
    "ADC",
    "ABS",
    "MAX",
    "MIN",
    "EMUL",
    "EMULU",
    "DIV",
    "DIVU",
    NULL,
    NULL,
    "TST",
    "XOR",
    "NOT",
    NULL
};

// AB.MI(C) opcodes
vchar *RXD_IA_AB_MI2_opcodes[] = {
    "SBB",
    NULL,
    "ADC",
    NULL,
    "MAX",
    "MIN",
    "EMUL",
    "EMULU",
    "DIV",
    "DIVU",
    NULL,
    NULL,
    "TST",
    "XOR",
    NULL,
    NULL
};

// STK.CTL opcodes
vchar *RXD_STK_CTL_opcodes[] = {
    "PUSHC",
    "POPC"
};

// Bit Manipulation Position, Register opcodes
vchar *RXD_BM_IR1_opcodes[] = {
    "BSET",
    "BCLR",
    "BTST",
    NULL
};

// BM.REG opcodes
vchar *RXD_BM_REG_opcodes[] = {
    "BSET",
    "BCLR",
    "BTST",
    "BNOT"
};

// BM.RNG opcodes
vchar *RXD_BM_RNG_opcodes[] = {
    "BFMOVZ",
    "BFMOV"
};

// 3-bit displacement
vint RXD_pcdsp3[] = {
    8,
    9,
    10,
    3,
    4,
    5,
    6,
    7
};

// 3-Bit PC-relative branch
vchar *RXD_CTRL_PCDSP3_opcodes[] = {
    NULL,
    "BRA.S",
    "BEQ.S",
    "BNE.S"
};

// 16-bit offset PC-relative branch opcodes
vchar *RXD_CTRL_PCDSP16_opcodes[] = {
    "BRA.W",
    "BSR.W",
    "BEQ.W",
    "BNE.W"
};

// 24-bit offset PC-relative branch opcodes
vchar *RXD_CTRL_PCDSP24_opcodes[] = {
    "BRA.A",
    "BSR.A"
};

// CTL.REG opcodes
vchar *RXD_CTRL_REG_opcodes[] = {
    "JMP",
    "JSR",
    "BRA.L",
    "BSR.L"
};

// FPU.I2F1/FPU.I2F2 opcodes (C0)
vchar *RXD_FPU_I2F_opcodes_v1[] = {
    "ITOF",
    NULL
};

vchar *RXD_FPU_I2F_opcodes_v2[] = {
    "ITOF",
    "UTOF"
};

// FPU.LD opcodes
vchar *RXD_FPU_LD_opcodes_v1[] = {
    "FSUB",
    "FCMP",
    "FADD",
    "FMUL",
    "FDIV",
    "FTOI",
    "ROUND",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// FPU.LD opcodes (v2)
vchar *RXD_FPU_LD_opcodes_v2[] = {
    "FSUB",
    "FCMP",
    "FADD",
    "FMUL",
    "FDIV",
    "FTOI",
    "ROUND",
    NULL,
    "FSQRT",
    "FTOU",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// FPU.IMM opcodes
vchar *RXD_FPU_IMM_opcodes[] = {
    "FSUB",
    "FCMP",
    "FADD",
    "FMUL",
    "FDIV",
    NULL,
    NULL,
    NULL
};
 
// FPU.3REG opcodes
vchar *RXD_FPU_3REG_opcodes[] = {
    "FSUB",
    NULL,
    "FADD",
    "FMUL"
};

// DPU Stack Opcodes
vchar *RXD_DPU_STK_opcodes[] = {
    "DPUSHM.L",
    "DPOPM.L",
    "DPUSHM.D",
    "DPOPM.D"
};

// To High opcodes
vchar *RXD_DPU_RTH_opcodes[] = {
    "DMOV.L",
    "DMOV.D"
};

// DPU Binary arithmetic
vchar *RXD_DPU_BA_opcodes[] = {
    "DADD",
    "DSUB",
    "DMUL",
    NULL,
    NULL,
    "DDIV",
    NULL,
    NULL
};

// DPU Unary Arithmetic/Movement 
vchar *RXD_DPU_UAM_opcodes[] = {
    "DMOV.D",
    "DABS",
    "DNEG",
    NULL,
    "DSQRT",
    NULL,
    NULL,
    NULL
};

// DPU Convert from double
vchar *RXD_DPU_CFD_opcodes[] = {
    "DTOI",
    "DTOU",
    "DTOF",
    "DROUND"
};

// DPU Convert to double
vchar *RXD_DPU_CTD_opcodes[] = {
    NULL,
    "ITOD",
    "FTOD",
    NULL,
    NULL,
    "UTOD",
    NULL,
    NULL
};

// DPU.CMP opcodes
vchar *RXD_DPU_CMP_opcodes[] = {
    NULL,
    "DCMPUN",
    "DCMPEQ",
    NULL,
    "DCMPLT",
    NULL,
    "DCMPLE",
    NULL
};

// DM.CMOV opcodes
vchar *RXD_DM_CMOV_opcodes[] = {
    "STZ",
    "STNZ"
};

//
// RXD_getSignedImmLen
//     Get length of signed immediate value
//

vint RXD_getSignedImmLen(vint li)
{
    return (li == 0) ? 4 : li;
}

//
// RXD_getDspLen
//     Get length of displacement coded in instruction
//

vint RXD_getDspLen(vint ld)
{
    return (ld == 1 || ld == 2) ? ld : 0;
}

//
// RXD_getDsp
//     Get displacement value based length of displacement (ld) coded in instruction
//

vint RXD_getDsp(vuint8 *inst, vint ld)
{
    vint dsp;

    if(ld == 1)
        dsp = inst[0];
    else if(ld == 2)
        dsp = inst[0] | ((vuint16)inst[1] << 8);
    else
        dsp = 0;

    return dsp;
}

//
// RXD_getSignedImm
//     Get signed immediate value based on length of immediate(li) coded in instruction. 
//
// Based on 6.3.1.3 of C Language Standard, if we assign an unsigned 32-bit value to 
// signed 32-bit value and the value cannot be represented in 32-bit signed value
// (0x80000000 to 0xFFFFFFFF), then what would be stored in 32-bit signed value is 
// implementation defined. However, if it is representable, we can store it without 
// loss. So, we type cast uimm to vint64 and then get the result.
//
// Based on 6.5.7 of C Language standard, result of left shift for signed integer when
// input is signed is undefined and result of signed right shift is implementation 
// defined
// https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
//
// According to gcc,
// https://gcc.gnu.org/onlinedocs/gcc/Integers-implementation.html
// Right shift of signed integer will result in signed extension
// Left shift is not undefined for signed data in signed integer
// 
// Below implementation doesn't make use of gcc definition, so it is portable.
//

vint32 RXD_getSignedImm(vuint8 *inst, vint bytesLeft, vint li)
{
    vint N;
    vint64 simm;
    vuint32 uimm;

    switch(li)
        {
        case 0:
            uimm = inst[0] | ((vuint32)inst[1] << 8) | ((vuint32)inst[2] << 16) | ((vuint32)inst[3] << 24);
            break;
        case 1:
            uimm = inst[0];
            break;
        case 2:
            uimm = inst[0] | ((vuint32)inst[1] << 8);
            break;
        case 3:
            uimm = inst[0] | ((vuint32)inst[1] << 8) | ((vuint32)inst[2] << 16);
            break;
        default:
            assert(0);
            break;
        }

    N = RXD_getSignedImmLen(li) * 8;
    simm = uimm;
    if(simm & ((vint64)1 << (N - 1)))
        simm = simm - ((vint64)1 << N);

    return simm;
}

//
// RXD_shift
//     Handling of Shift related operation
//

vint RXD_shift(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_ShiftGrp grp)
{
    vint Rs, Rs2, Rd, sh, code, len;

    switch(grp)
        {
        case RX_GRP_IA_SH_REG1:
            Rs = (inst[2] >> 4) & 15;
            Rd = (inst[2] & 15);
            code = inst[1] & 7;
            assert(RXD_IALU_SH_REG1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IALU_SH_REG1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_IA_SH_REG2:
            Rs2 = (inst[2] >> 4) & 15;
            Rd = (inst[2] & 15);
            sh = inst[1] & 31;
            code = (inst[1] >> 5) & 3;
            assert(RXD_IALU_SH_REG2_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IALU_SH_REG2_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", sh);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            snprintf(opc[3], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 3;
            len = 3;
            break;
        case RX_GRP_IA_SH_IMM1:
            Rd = inst[1] & 15;
            sh = ((inst[1] >> 4) & 15) | ((inst[0] & 1) << 4);
            code = (inst[0] >> 1) & 3;
            assert(RXD_IALU_SH_IMM1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IALU_SH_IMM1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", sh);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_IA_SH_IMM2:
            Rd = inst[2] & 15;
            sh = ((inst[2] >> 4) & 15) | ((inst[1] & 1) << 4);
            code = (inst[1] >> 1) & 1;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IALU_SH_IMM2_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", sh);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_IA_SH_CRY:
            code = (inst[1] >> 4) & 1;
            Rd = inst[1] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IALU_SH_CRY_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 1;
            len = 2;
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_IALU_imm
//     Handling of Integer ALU with immediate operands
//

vint RXD_IALU_imm(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_IA_IR_Grp grp)
{
    vint32 simm;
    vuint32 uimm4, uimm8;
    vint len, li, Rs2, Rd, code;

    switch(grp)
        {
        case RX_GRP_IA_AB_IR1:
            // <opc> #uimm4, reg
            code = inst[0] & 7;
            uimm4 = (inst[1] >> 4) & 15;
            Rd = inst[1] & 15;
            assert(RXD_IA_AB_IR1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_IR1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", uimm4);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_IA_AB_IR2C:
            // <opc> #uimm8, reg
            Rs2 = inst[1] & 15;
            uimm8 = inst[2];
            snprintf(opc[0], RX_MAX_OP_LEN, "CMP");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", uimm8);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_IA_AB_IR3:
            // <opc> #imm, reg (2 byte opcode)
            li = inst[0] & 3;
            code = (inst[1] >> 4) & 3;
            Rd = inst[1] & 15;
            simm = RXD_getSignedImm(inst + 2, bytesLeft - 2, li);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_IR3_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)simm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2 + RXD_getSignedImmLen(li);
            break;
        case RX_GRP_IA_AB_IR4:
            // <opc> #imm, reg (3 byte opcode)
            li = (inst[1] >> 2) & 3;
            code = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            simm = RXD_getSignedImm(inst + 3, bytesLeft - 3, li);
            assert(RXD_IA_AB_IR4_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_IR4_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)simm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3 + RXD_getSignedImmLen(li);
            break;
        case RX_GRP_IA_AB_IR5:
            // <opc> #imm, reg, reg
            li = inst[0] & 3;
            Rs2 = (inst[1] >> 4) & 15;
            Rd = inst[1] & 15;
            simm = RXD_getSignedImm(inst + 2, bytesLeft - 2, li);
            snprintf(opc[0], RX_MAX_OP_LEN, "ADD");
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)simm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            snprintf(opc[3], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 3;
            len = 2 + RXD_getSignedImmLen(li);
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_IALU_reg
//     Handling of Integer ALU with register only operands
//
 
vint RXD_IALU_reg(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_IA_REG_Grp grp)
{
    vint len, Rs, Rs2, Rd, code;
 
    switch(grp)
        {
        case RX_GRP_IA_AB_REG1:
            // <opc> Rd
            Rd = inst[1] & 15;
            code = (inst[1] >> 4) & 3;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_REG1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_IA_AB_REG2:
            // <opc> Rs, Rs2, Rd
            Rd = inst[1] & 15;
            Rs2 = inst[2] & 15;
            Rs = (inst[2] >> 4) & 15;
            code = (inst[1] >> 4) & 7;
            if(version == 3)
                {
                assert(RXD_IA_AB_REG2_opcodes_v3[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_REG2_opcodes_v3[code]);
                }
            else
                {
                assert(RXD_IA_AB_REG2_opcodes_v1[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_REG2_opcodes_v1[code]);
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            snprintf(opc[3], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 3;
            len = 3;
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_IALU_rmem
//     Handling of Integer ALU with register/memory source operand and register destination
//
 
vint RXD_IALU_rmem(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_IA_RMEM_Grp grp)
{
    vint len, ld, mi, Rs, Rd, code;

    switch(grp)
        {
        case RX_GRP_IA_AB_LD1:
            // <opc> Rs/[Rs].UB/dsp[Rs].UB, Rd (2 byte opcode)
            ld = inst[0] & 3;
            Rs = (inst[1] >> 4) & 15;
            Rd = (inst[1] & 15);
            code = (inst[0] >> 2) & 7;
            assert(RXD_IA_AB_RMEM1_opcodes[code] != NULL);

            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_RMEM1_opcodes[code]);
            if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].UB", RXD_getDsp(inst + 2, ld), Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].UB", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(ld);
            break;
        case RX_GRP_IA_AB_LD2:
            // <opc> Rs/[Rs].UB/dsp[Rs].UB, Rd (3 byte opcode)
            code = (inst[1] >> 2) & 15;
            ld = inst[1] & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            assert(RXD_IA_AB_LD2_opcodes[code] != NULL);
            if(code <= 3 || code == 14)
                assert(ld == 3);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_LD2_opcodes[code]);
            if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].UB", RXD_getDsp(inst + 3, ld), Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].UB", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);

            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_IA_AB_MI1:
            // <opc> [Rs].mi/dsp[Rs].mi, Rd (3 byte opcode)
            mi = (inst[1] >> 6) & 3;
            code = (inst[1] >> 2) & 7;
            ld = inst[1] & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            assert(RXD_IA_AB_RMEM1_opcodes[code] != NULL);
            assert(ld != 3);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_RMEM1_opcodes[code]);
            if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].%s", RXD_getDsp(inst + 3, ld) * RXD_MI_scale[mi], Rs, RXD_MI_str[mi]);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].%s", Rs, RXD_MI_str[mi]);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);

            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_IA_AB_MI2:
            // <opc> [Rs].mi/dsp[Rs].mi, Rd (4 byte opcode)
            mi = (inst[1] >> 6) & 3;
            ld = inst[1] & 3;
            code = inst[2] & 15;
            Rs = (inst[3] >> 4) & 15;
            Rd = inst[3] & 15;
            assert(RXD_IA_AB_MI2_opcodes[code] != NULL);
            assert(ld != 3);
            if(code == 0 || code == 2)
                assert(mi == 2);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_IA_AB_MI2_opcodes[code]);
            if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].%s", RXD_getDsp(inst + 4, ld) * RXD_MI_scale[mi], Rs, RXD_MI_str[mi]);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].%s", Rs, RXD_MI_str[mi]);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);

            *opCnt = 2;
            len = 4 + RXD_getDspLen(ld);
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_ldst
//     Handling of load and store instructions
//
 
vint RXD_ldst(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_LdStGrp grp)
{
    vint len, sz, ld, ad, dsp, Rs, Rd, Ri, Rb;
 
    switch(grp)
        {
        case RX_GRP_LD_SOFF_S:
        case RX_GRP_LD_SOFF_U:
            // Load Signed/Unsigned with small offset
            Rd = inst[1] & 7;
            Rs = (inst[1] >> 4) & 7;
            dsp = ((inst[0] & 7) << 2) | ((inst[1] >> 6) & 2) | ((inst[1] >> 3) & 1);
            if(grp == RX_GRP_LD_SOFF_U)
                {
                sz = (inst[0] >> 3) & 1;
                snprintf(opc[0], RX_MAX_OP_LEN, "MOVU.%s", RXD_SZ_str[sz]);
                }
            else
                {
                sz = (inst[0] >> 4) & 3;
                assert(RXD_SZ_str[sz] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", dsp << sz, Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_ST_SOFF:
            // Store with small offset
            Rs = inst[1] & 7;
            Rd = (inst[1] >> 4) & 7;
            sz = (inst[0] >> 4) & 3;
            assert(RXD_SZ_str[sz] != NULL);
            dsp = ((inst[0] & 7) << 2) | ((inst[1] >> 6) & 2) | ((inst[1] >> 3) & 1);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", dsp << sz, Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_LD_OFF_S:
        case RX_GRP_LD_OFF_U:
            Rs = (inst[1] >> 4) & 15;
            Rd = inst[1] & 15;
            ld = inst[0] & 3;
            if(grp == RX_GRP_LD_OFF_S)
                {
                sz = (inst[0] >> 4) & 3;
                assert(ld != 3 && RXD_SZ_str[sz] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
                }
            else
                {
                sz = (inst[0] >> 2) & 1;
                snprintf(opc[0], RX_MAX_OP_LEN, "MOVU.%s", RXD_SZ_str[sz]);
                }
            if(ld == 3)
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2, ld) << sz, Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(ld);
            break;
        case RX_GRP_ST_OFF:
            Rd = (inst[1] >> 4) & 15;
            Rs = inst[1] & 15;
            ld = (inst[0] >> 2) & 3;
            sz = (inst[0] >> 4) & 3;
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            assert(ld != 3);
            if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d]", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2, ld) << sz, Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(ld);
            break;
        case RX_GRP_ST_IDX:
        case RX_GRP_LD_IDX_S:
        case RX_GRP_LD_IDX_U:
            Ri = inst[1] & 15;
            Rb = (inst[2] >> 4) & 15;
            Rs = Rd = inst[2] & 15;
            if(grp == RX_GRP_LD_IDX_U)
                {
                sz = (inst[1] >> 4) & 1;
                snprintf(opc[0], RX_MAX_OP_LEN, "MOVU.%s", RXD_SZ_str[sz]);
                }
            else
                {
                sz = (inst[1] >> 4) & 3;
                assert(RXD_SZ_str[sz] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
                }

            if(grp == RX_GRP_ST_IDX)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d, R%d]", Ri, Rb);
                }
            else
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d, R%d]", Ri, Rb);
                snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
                }
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_ST_SEQ_S:
            ad = (inst[1] >> 2) & 3;
            sz = inst[1] & 3;
            Rd = (inst[2] >> 4) & 15;
            Rs = inst[2] & 15;
            assert(RXD_SZ_str[sz] != NULL);
            assert(ad == 0 || ad == 1);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, (ad == 0) ? "[R%d+]" : "[-R%d]", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_LD_SEQ_S:
        case RX_GRP_LD_SEQ_U:
            ad = (inst[1] >> 2) & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            assert(ad == 2 || ad == 3);
            if(grp == RX_GRP_LD_SEQ_S)
                {
                sz = inst[1] & 3;
                assert(RXD_SZ_str[sz] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
                }
            else
                {
                sz = inst[1] & 1;
                snprintf(opc[0], RX_MAX_OP_LEN, "MOVU.%s", RXD_SZ_str[sz]);
                }
            snprintf(opc[1], RX_MAX_OP_LEN, (ad == 2) ? "[R%d+]" : "[-R%d]", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_stack
//     Handling of stack operations. 
//
 
vint RXD_stack(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_StkGrp grp)
{
    vint len, code, sz, ld, Rs, Rs2, Rd, Rd2, Cr;
 
    switch(grp)
        {
        case RX_GRP_STK_MI:
        case RX_GRP_STK_MO:
            Rd = Rs = (inst[1] >> 4) & 15;
            Rd2 = Rs2 = inst[1] & 15;
            if(grp == RX_GRP_STK_MI)
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "PUSHM");
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d-R%d", Rs, Rs2);
                }
            else
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "POPM");
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d-R%d", Rd, Rd2);
                }
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_STK_SI:
        case RX_GRP_STK_SO:
            sz = (inst[1] >> 4) & 3;
            Rd = Rs = inst[1] & 15;
            if(grp == RX_GRP_STK_SI)
                {
                assert(RXD_SZ_str[sz] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "PUSH.%s", RXD_SZ_str[sz]);
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
                }
            else
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "POP");
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
                }
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_STK_CTL:
            code = (inst[1] >> 5) & 1;
            Cr = inst[1] & 15;
            assert(RXD_Cr_str[Cr] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_STK_CTL_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "%s", RXD_Cr_str[Cr]);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_STK_SIM:
            ld = inst[0] & 3;
            Rs = (inst[1] >> 4) & 15;
            sz = inst[1] & 3;
            assert(ld != 3 && RXD_SZ_str[sz] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "PUSH.%s", RXD_SZ_str[sz]);
            if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2, ld) << sz, Rs);
            *opCnt = 1;
            len = 2 + RXD_getDspLen(ld);
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_dm
//     Handling of Data Movement related operation
//

vint RXD_dm(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_DataMvGrp grp)
{
    vint32 imm;
    vint Rs, Rd, mi, li, ld, sz, code, len, cd, dsp, ldd, lds;

    switch(grp)
        {
        case RX_GRP_DM_IRS:
            // Immediate (Small) to register
            Rd = inst[1] & 15;
            imm = inst[2];
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.L");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", imm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_DM_IR:
            // Immediate to register
            Rd = (inst[1] >> 4) & 15;
            li = (inst[1] >> 2) & 3;
            imm = RXD_getSignedImm(inst + 2, bytesLeft - 2, li);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.L");
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)imm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2 + RXD_getSignedImmLen(li);
            break;
        case RX_GRP_DM_RR:
            // Register to Register
            sz = (inst[0] >> 4) & 3;
            Rs = (inst[1] >> 4) & 15;
            Rd = inst[1] & 15;
            assert(RXD_SZ_str[sz] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_DM_IMS:
            // Immediate to memory short displacement and immediate
            Rd = (inst[1] >> 4) & 7;
            dsp = (inst[1] & 15) | ((inst[1] >> 3) & 0x10);
            imm = inst[2];
            sz = inst[0] & 3;
            assert(RXD_SZ_str[sz] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", imm);
            snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", dsp << sz, Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_DM_IM:
            // Immediate to memory
            sz = inst[1] & 3;
            li = (inst[1] >> 2) & 3;
            Rd = (inst[1] >> 4) & 15;
            ld = inst[0] & 3;
            assert(ld != 3);
            assert(RXD_SZ_str[sz] != NULL);
            if(sz == 0)
                assert(li == 1);
            else 
                {
                if(sz == 1)
                    assert(li == 1 || li == 2);
                }
            imm = RXD_getSignedImm(inst + 2 + RXD_getDspLen(ld), bytesLeft - 3, li);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)imm);
            if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d]", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2, ld) << sz, Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(ld) + RXD_getSignedImmLen(li);
            break;
        case RX_GRP_DM_MM:
            // Memory to Memory
            sz = (inst[0] >> 4) & 3;
            ldd = (inst[0] >> 2) & 3;
            lds = inst[0] & 3;
            Rs = (inst[1] >> 4) & 15;
            Rd = inst[1] & 15;
            assert(ldd != 3 && lds != 3 && RXD_SZ_str[sz] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MOV.%s", RXD_SZ_str[sz]);
            if(lds == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2, lds) << sz, Rs);
            if(ldd == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d]", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 2 + RXD_getDspLen(lds), ldd) << sz, Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(lds) + RXD_getDspLen(ldd);
            break;
        case RX_GRP_DM_CMOV:
            // Conditional Move
            assert(version > 1);
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            code = (inst[1] >> 2) & 1;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DM_CMOV_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_DM_CRES:
            // Condition Result
            sz = (inst[1] >> 2) & 3;
            ld = inst[1] & 3;
            cd = inst[2] & 15;
            Rd = (inst[2] >> 4) & 15;
            assert(RXD_SZ_str[sz] != NULL);
            assert(RXD_COND_str[cd] != NULL);
            if(ld == 3)
                assert(sz == 2);
            
            snprintf(opc[0], RX_MAX_OP_LEN, "SC%s.%s", RXD_COND_str[cd], RXD_SZ_str[sz]);
            if(ld == 3)
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
            else if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 3, ld) << sz, Rd);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", Rd);
            *opCnt = 1;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_DM_EX_LD:
            // Exclusive Load
            assert(version > 1);
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "MOVLI");
            snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_DM_EX_ST:
            // Exclusive Store
            assert(version > 1);
            Rd = (inst[2] >> 4) & 15;
            Rs = inst[2] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "MOVCO");
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "[R%d]", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_DM_XCHG_LD:
            // Exchange with register/UB
            ld = inst[1] & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "XCHG");
            if(ld == 3)
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            else if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].UB", RXD_getDsp(inst + 3, ld), Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].UB", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_DM_XCHG_MI:
            // Exchange with other memory extensions
            ld = inst[1] & 3;
            mi = (inst[1] >> 6) & 3;
            Rs = (inst[3] >> 4) & 15;
            Rd = inst[3] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "XCHG");
            assert(ld != 3);

            if(ld == 1 || ld == 2)
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].%s", RXD_getDsp(inst + 4, ld) *  RXD_MI_scale[mi], Rs, RXD_MI_str[mi]);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].%s", Rs, RXD_MI_str[mi]);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 4 + RXD_getDspLen(ld);
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_array
//     Handling of Array related operation
//

vint RXD_array(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_ArrayGrp grp)
{
    vint code, sz;

    sz = (inst[1] & 3);
    code = (inst[1] >> 2) & 3;
    
    switch(grp)
        {
        case RX_GRP_ARRAY_SZ:
            // sz is guaranteed to be not 3 by caller, so no assert
            snprintf(opc[0], RX_MAX_OP_LEN, "%s.%s", RXD_ARY_SZ_opcodes[code], RXD_SZ_str[sz]);
            break;
        case RX_GRP_ARRAY_BYTE:
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_ARY_B_opcodes[code]);
            break;
        default:
            assert(0);
            break;
        }

    *opCnt = 0;
    return 2;
}

//
// RXD_bit
//     Handling of bit manipulation instructions. 
//
 
vint RXD_bit(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_BMGrp grp)
{
    vint len, code, bpos, cd, ld, Rs, Rd, dlsb, slsb, width, imm;
 
    switch(grp)
        {
        case RX_GRP_BM_IR1:
        case RX_GRP_BM_IR1C:
            bpos = ((inst[0] & 1) << 4) | ((inst[1] >> 4) & 15);
            Rd = Rs = inst[1] & 15;
            code = (inst[0] >> 1) & 3;
            assert(RXD_BM_IR1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_BM_IR1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", bpos);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 2;
            break;
        case RX_GRP_BM_IR2:
        case RX_GRP_BM_IR3:
            bpos = inst[1] & 31;
            cd = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            if(grp == RX_GRP_BM_IR2)
                {
                assert(RXD_COND_str[cd] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "BM%s", RXD_COND_str[cd]);
                }
            else
                snprintf(opc[0], RX_MAX_OP_LEN, "BNOT");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", bpos);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_BM_IM1:
        case RX_GRP_BM_IM1C:
            ld = inst[0] & 3;
            bpos = inst[1] & 7;
            Rd = (inst[1] >> 4) & 15;
            code = ((inst[0] & 4) >> 1) | ((inst[1] >> 3) & 1);
            assert(ld != 3 && RXD_BM_IR1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_BM_IR1_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", bpos);
            if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d].B", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d].B", RXD_getDsp(inst + 2, ld), Rd);
            *opCnt = 2;
            len = 2 + RXD_getDspLen(ld);
            break;
        case RX_GRP_BM_IM2:
        case RX_GRP_BM_IM3:
            ld = (inst[1] & 3);
            bpos = (inst[1] >> 2) & 7;
            Rd = (inst[2] >> 4) & 15;
            cd = inst[2] & 15;
            if(grp == RX_GRP_BM_IM2)
                {
                assert(RXD_COND_str[cd] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "BM%s", RXD_COND_str[cd]);
                }
            else
                snprintf(opc[0], RX_MAX_OP_LEN, "BNOT");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", bpos);
            assert(ld != 3);
            if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d].B", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d].B", RXD_getDsp(inst + 3, ld), Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_BM_REG:
            code = (inst[1] >> 2) & 3;
            ld = inst[1] & 3;
            Rd = (inst[2] >> 4) & 15;
            Rs = inst[2] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_BM_REG_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            if(ld == 3)
                snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            else if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d].B", Rd);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d].B", RXD_getDsp(inst + 3, ld), Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_BM_RNG:
            assert(version == 3);
            code = (inst[1] >> 2) & 1;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            imm = inst[3] | ((vint)inst[4] << 8);
            dlsb = (imm >> 5) & 31;
            slsb = (dlsb - (imm & 31)) & 31;
            width = (((imm >> 10) & 31) - dlsb) & 31;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_BM_RNG_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", slsb);
            snprintf(opc[2], RX_MAX_OP_LEN, "#%d", dlsb);
            snprintf(opc[3], RX_MAX_OP_LEN, "#%d", width);
            snprintf(opc[4], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[5], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 5;
            len = 5;
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_dsp
//     Handling of DSP (accumulator) related operation. RXv1 has a single accumulator. So Accumulator
// is an implicit operand. RXv2 and v3 have two accumulators. So, accumulator is an operand. Also
// instructions and operand availability is limited as noted in the code
//
 
vint RXD_dsp(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_DspGrp grp)
{
    vint code, A, Rs, Rs2, Rd, imm1, imm2;
 
    switch(grp)
        {
        case RX_GRP_DSP_MUL:
            code = (((inst[1] >> 6) & 1) << 3) | (inst[1] & 7);
            A = (inst[1] >> 3) & 1;
            Rs = (inst[2] >> 4) & 15;
            Rs2 = inst[2] & 15;
            if(version == 1)
                {
                assert(RXD_DSP_MUL_opcodes_v1[code] != NULL);
                assert(A == 0);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_MUL_opcodes_v1[code]);
                *opCnt = 2;
                }
            else
                {
                assert(RXD_DSP_MUL_opcodes_v2[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_MUL_opcodes_v2[code]);
                snprintf(opc[3], RX_MAX_OP_LEN, "A%d", A);
                *opCnt = 3;
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            break;
        case RX_GRP_DSP_TAC:
            A = (inst[2] >> 7) & 1;
            code = (inst[2] >> 4) & 3;
            Rs = inst[2] & 15;
            if(version == 1)
                {
                assert(RXD_DSP_TAC_opcodes_v1[code] != NULL);
                assert(A == 0);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_TAC_opcodes_v1[code]);
                *opCnt = 1;
                }
            else
                {
                assert(RXD_DSP_TAC_opcodes_v2[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_TAC_opcodes_v2[code]);
                snprintf(opc[2], RX_MAX_OP_LEN, "A%d", A);
                *opCnt = 2;
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            break;
        case RX_GRP_DSP_FAC:
            A = (inst[2] >> 7) & 1;
            code = (inst[2] >> 4) & 3;
            Rd = inst[2] & 15;
            imm2 = ((inst[1] & 1) << 1) | ((inst[2] >> 6) & 1);
            assert(imm2 != 1);
            imm2 = (imm2 == 0) ? 2 : imm2 - 2;
            if(version == 1)
                {
                assert(RXD_DSP_FAC_opcodes_v1[code] != NULL);
                assert(A == 0 && imm2 == 0);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_FAC_opcodes_v1[code]);
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rd);
                *opCnt = 1;
                }
            else
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_FAC_opcodes_v2[code]);
                snprintf(opc[1], RX_MAX_OP_LEN, "#%d", imm2);
                snprintf(opc[2], RX_MAX_OP_LEN, "A%d", A);
                snprintf(opc[3], RX_MAX_OP_LEN, "R%d", Rd);
                *opCnt = 3;
                }
            break;
        case RX_GRP_DSP_RND:
            code = ((inst[1] & 1) << 1) | ((inst[2] >> 6) & 1);
            A = (inst[2] >> 7) & 1;
            imm1 = ((inst[2] >> 4) & 1) + 1;
            if(version == 1)
                {
                assert(A == 0);
                assert(RXD_DSP_RND_opcodes_v1[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_RND_opcodes_v1[code]);
                *opCnt = 1;
                }
            else
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DSP_RND_opcodes_v2[code]);
                snprintf(opc[2], RX_MAX_OP_LEN, "A%d", A);
                *opCnt = 2;
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", imm1);
            break;
        default:
            assert(0);
            break;
        }
 
    return 3;
}

//
// RXD_ctrl
//     Control transfer instructions
//

vint RXD_ctrl(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_CtrlGrp grp)
{
    vint32 imm;
    vint len, code, Rs, Rd, Rd2;
 
    switch(grp)
        {
        case RX_GRP_CTRL_IMM_S:
            imm = inst[0] & 7;
            code = (inst[0] >> 3) & 3;
            assert(RXD_CTRL_PCDSP3_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_CTRL_PCDSP3_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "PC + 0%08Xh", RXD_pcdsp3[imm]);
            *opCnt = 1;
            len = 1;
            break;
        case RX_GRP_CTRL_IMM_B:
            code = inst[0] & 15;
            imm = inst[1];
            if(imm & 0x80)
                imm = imm - 0x100;
            if(code == 14)
                snprintf(opc[0], RX_MAX_OP_LEN, "BRA.B");
            else
                {
                assert(RXD_COND_str[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "B%s.B", RXD_COND_str[code]);
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "PC + 0%08Xh", imm);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_CTRL_IMM_W:
            code = inst[0] & 3;
            imm = inst[1] | ((vuint32)inst[2] << 8);
            if(imm & 0x8000)
                imm = imm - ((vint32)1 << 16);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_CTRL_PCDSP16_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "PC + 0%08Xh", imm);
            *opCnt = 1;
            len = 3;
            break;
        case RX_GRP_CTRL_IMM_A:
            code = inst[0] & 1;
            imm = inst[1] | ((vuint32)inst[2] << 8) | ((vuint32)inst[3] << 16);
            if(imm & 0x800000)
                imm = imm - ((vint32)1 << 24);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_CTRL_PCDSP24_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "0%08Xh", imm);
            *opCnt = 1;
            len = 4;
            break;
        case RX_GRP_CTRL_REG:
            code = ((inst[1] >> 5) & 2) | ((inst[1] >> 4) & 1);
            Rs = inst[1] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_CTRL_REG_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_CTRL_ACA:
            snprintf(opc[0], RX_MAX_OP_LEN, "RTSD");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", inst[1] << 2);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_CTRL_ACR:
            Rd = (inst[1] >> 4) & 15;
            Rd2 = inst[1] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "RTSD");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", inst[2] << 2);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d-R%d", Rd, Rd2);
            *opCnt = 2;
            len = 3;
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_system
//     Handling of System related operation
//

vint RXD_system(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_SysGrp grp)
{
    vint32 simm;
    vint intr, ipl, flag, code, len, Rs, Rd, Cr, li;

    switch(grp)
        {
        case RX_GRP_SYS_MTC:
            Rs = (inst[2] >> 4) & 15;
            Cr = inst[2] & 15;
            assert(RXD_Cr_str[Cr] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MVTC");
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "%s", RXD_Cr_str[Cr]);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_SYS_MFC:
            Cr = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            assert(RXD_Cr_str[Cr] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MVFC");
            snprintf(opc[1], RX_MAX_OP_LEN, "%s", RXD_Cr_str[Cr]);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3;
            break;
        case RX_GRP_SYS_MTCI:
            li = (inst[1] >> 2) & 3;
            Cr = inst[2] & 15;
            simm = RXD_getSignedImm(inst + 3, bytesLeft - 3, li);
            assert(RXD_Cr_str[Cr] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "MVTC");
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", (vuint32)simm);
            snprintf(opc[2], RX_MAX_OP_LEN, "%s", RXD_Cr_str[Cr]);
            *opCnt = 2;
            len = 3 + RXD_getSignedImmLen(li);
            break;
        case RX_GRP_SYS_IPL:
            ipl = inst[2] & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "MVTIPL");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", ipl);
            *opCnt = 1;
            len = 3;
            break;
        case RX_GRP_SYS_INT:
            intr = inst[2];
            snprintf(opc[0], RX_MAX_OP_LEN, "INT");
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", intr);
            *opCnt = 1;
            len = 3;
            break;
        case RX_GRP_SYS_PSW:
            code = (inst[1] >> 4) & 1;
            flag = inst[1] & 15;
            assert(RXD_flag_str[flag] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_SYS_PSW_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "%s", RXD_flag_str[flag]);
            *opCnt = 1;
            len = 2;
            break;
        case RX_GRP_SYS_RBR:
            assert(version == 3);
            Rs = inst[2] & 15;
            code = (inst[2] >> 4) & 1;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_RB_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            *opCnt = 1;
            len = 4;
            break;
        case RX_GRP_SYS_RBI:
            assert(version == 3);
            simm = inst[3];
            code = (inst[2] >> 4) & 1;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_RB_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#%d", simm);
            *opCnt = 1;
            len = 4;
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_zero
//     Handling of Zero Operand Instruction Groups
//

vint RXD_zero(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_ZeroOpGrp grp)
{
    vint code, len;

    switch(grp)
        {
        case RX_GRP_ZERO_OP1:
            code = inst[1] & 7;
            assert(RXD_ZERO_OP1_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_ZERO_OP1_opcodes[code]);
            *opCnt = 0;
            len = 2;
            break;
        case RX_GRP_ZERO_OP2:
            code = inst[0] & 3;
            assert(RXD_ZERO_OP2_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_ZERO_OP2_opcodes[code]);
            *opCnt = 0;
            len = 1;
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_fpu
//     Handling of FPU instructions. 
//
 
vint RXD_fpu(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_FpuGrp grp)
{
    vuint32 imm;
    vint len, code, ld, mi, Rs, Rs2, Rd;
 
    switch(grp)
        {
        case RX_GRP_FPU_I2F_LD:
            code = (inst[1] >> 4) & 1;
            ld = inst[1] & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            if(version == 1)
                {
                assert(RXD_FPU_I2F_opcodes_v1[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_I2F_opcodes_v1[code]);
                }
            else
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_I2F_opcodes_v2[code]);
            if(ld == 3)
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].UB", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].UB", RXD_getDsp(inst + 3, ld), Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_FPU_I2F_MI:
            mi = (inst[1] >> 6) & 3;
            ld = inst[1] & 3;
            code = (inst[2] >> 2) & 1;
            Rs = (inst[3] >> 4) & 15;
            Rd = inst[3] & 15;
            assert(ld != 3);
            if(version == 1)
                {
                assert(RXD_FPU_I2F_opcodes_v1[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_I2F_opcodes_v1[code]);
                }
            else
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_I2F_opcodes_v2[code]);
            if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].%s", Rs, RXD_MI_str[mi]);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].%s", RXD_getDsp(inst + 4, ld) * RXD_MI_scale[mi], Rs, RXD_MI_str[mi]);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 4 + RXD_getDspLen(ld);
            break;
        case RX_GRP_FPU_LD:
            code = (inst[1] >> 2) & 15;
            ld = inst[1] & 3;
            Rs = (inst[2] >> 4) & 15;
            Rd = inst[2] & 15;
            if(version == 1)
                {
                assert(RXD_FPU_LD_opcodes_v1[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_LD_opcodes_v1[code]);
                }
            else
                {
                assert(RXD_FPU_LD_opcodes_v2[code] != NULL);
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_LD_opcodes_v2[code]);
                }
            if(ld == 3)
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            else if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d].L", Rs);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d].L", RXD_getDsp(inst + 3, ld) << 2, Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 3 + RXD_getDspLen(ld);
            break;
        case RX_GRP_FPU_IMM:
            code = (inst[2] >> 4) & 7;
            Rd = inst[2] & 15;
            imm = inst[3] | ((vuint32)inst[4] << 8) | ((vuint32)inst[5] << 16) | ((vuint32)inst[6] << 24);
            assert(RXD_FPU_IMM_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_IMM_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", imm);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 2;
            len = 7;
            break;
        case RX_GRP_FPU_3REG:
            code = (inst[1] >> 4) & 3;
            Rd = inst[1] & 15;
            Rs = (inst[2] >> 4) & 15;
            Rs2 = inst[2] & 15;
            assert(version > 1 && RXD_FPU_3REG_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_FPU_3REG_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", Rs);
            snprintf(opc[2], RX_MAX_OP_LEN, "R%d", Rs2);
            snprintf(opc[3], RX_MAX_OP_LEN, "R%d", Rd);
            *opCnt = 3;
            len = 3;
            break;
        default:
            assert(0);
            break;
        }
 
    return len;
}

//
// RXD_dpu
//     Handling of DPU instructions. For load/store with offset, the scale factor is recorded
// 4 and 8 in instruction reference, encoding of the manual. So, the manual contradicts itself.
// We have used ccrx and tried with 64k*4 as offset. It gave error that value is out of range.
// We have used it to conclude the scale factor is indeed 4.
//
 
vint RXD_dpu(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt, RXD_DpuGrp grp)
{
    vuint32 uimm;
    vint len, code, ld, nm, reg, reg1, reg2, drs, drs2, drd;

    assert(version == 3);
    switch(grp)
        {
        case RX_GRP_DPU_BA:
            // Binary Arithmetic - <opc> DRs, DRs1, DRd
            code = inst[2] & 7;
            drd = (inst[3] >> 4) & 15;
            drs = (inst[3] & 15);
            drs2 = (inst[2] >> 4) & 15;
            assert(RXD_DPU_BA_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_BA_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "DR%d", drs);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", drs2);
            snprintf(opc[3], RX_MAX_OP_LEN, "DR%d", drd);
            *opCnt = 3;
            len = 4;
            break;
        case RX_GRP_DPU_UAM:
            // Unary Arithmetic - <opc> DRs, DRd
            code = (inst[3] & 3) | ((inst[2] & 1) << 2);
            drs = (inst[2] >> 4) & 15;
            drs2 = (inst[3] >> 4) & 15;
            assert(RXD_DPU_UAM_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_UAM_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "DR%d", drs);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", drs2);
            *opCnt = 2;
            len = 4;
            break;
        case RX_GRP_DPU_CMP:
            // Comparison
            code = (inst[3] >> 4) & 7;
            reg2 = (inst[2] >> 4) & 15;
            reg1 = (inst[3] & 15);
            assert(RXD_DPU_CMP_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_CMP_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "DR%d", reg1);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", reg2);
            *opCnt = 2;
            len = 4;
            break;
        case RX_GRP_DPU_PUSHC:
        case RX_GRP_DPU_POPC:
        case RX_GRP_DPU_PUSH:
        case RX_GRP_DPU_POP:
            // Stack Operations
            nm = inst[2] & 15;
            code = (inst[1] >> 3) & 3;
            reg = (inst[2] >> 4) & 15;

            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_STK_opcodes[code]);
            if((code >> 1) == 0)
                {
                assert(reg + nm < 4);
                snprintf(opc[1], RX_MAX_OP_LEN, "DCR%d-DCR%d", reg, reg + nm);
                }
            else
                {
                assert(reg + nm < 16);
                snprintf(opc[1], RX_MAX_OP_LEN, "DR%d-DR%d", reg, reg + nm);
                }
            *opCnt = 1;
            len = 3;
            break;
        case RX_GRP_DPU_RFL:
        case RX_GRP_DPU_RFH:
        case RX_GRP_DPU_RFC:
        case RX_GRP_DPU_RTL:
        case RX_GRP_DPU_RTH:
        case RX_GRP_DPU_RTC:
            // Move from and to b/w registers
            reg1 = inst[2] & 15;
            reg2 = (inst[3] >> 4) & 15;
            code = inst[3] & 1;
            if(grp == RX_GRP_DPU_RTH)
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_RTH_opcodes[code]);
            else if(grp == RX_GRP_DPU_RFC)
                snprintf(opc[0], RX_MAX_OP_LEN, "MVFDC");
            else if(grp == RX_GRP_DPU_RTC)
                snprintf(opc[0], RX_MAX_OP_LEN, "MVTDC");
            else
                snprintf(opc[0], RX_MAX_OP_LEN, "DMOV.L");

            if(grp == RX_GRP_DPU_RFL)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "DRL%d", reg2);
                snprintf(opc[2], RX_MAX_OP_LEN, "R%d", reg1);
                }
            else if(grp == RX_GRP_DPU_RFH)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "DRH%d", reg2);
                snprintf(opc[2], RX_MAX_OP_LEN, "R%d", reg1);
                }
            else if(grp == RX_GRP_DPU_RFC)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "DCR%d", reg2);
                snprintf(opc[2], RX_MAX_OP_LEN, "R%d", reg1);
                }
            else if(grp == RX_GRP_DPU_RTL)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", reg1);
                snprintf(opc[2], RX_MAX_OP_LEN, "DRL%d", reg2);
                }
            else if(grp == RX_GRP_DPU_RTH)
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", reg1);
                snprintf(opc[2], RX_MAX_OP_LEN, "DRH%d", reg2);
                }
            else
                {
                snprintf(opc[1], RX_MAX_OP_LEN, "R%d", reg1);
                snprintf(opc[2], RX_MAX_OP_LEN, "DCR%d", reg2);
                }
            *opCnt = 2;
            len = 4;
            break;
        case RX_GRP_DPU_RTLI:
        case RX_GRP_DPU_RTHI:
            // Move from immediate to register
            reg = (inst[2] >> 4) & 15;
            uimm = inst[3] | ((vuint32)inst[4] << 8) | ((vuint32)inst[5] << 16) | ((vuint32)inst[6] << 24);
            if(grp == RX_GRP_DPU_RTLI)
                {
                snprintf(opc[0], RX_MAX_OP_LEN, "DMOV.L");
                snprintf(opc[2], RX_MAX_OP_LEN, "DRL%d", reg);
                }
            else
                {
                code = inst[2] & 1;
                snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_RTH_opcodes[code]);
                snprintf(opc[2], RX_MAX_OP_LEN, "DRH%d", reg);
                }
            snprintf(opc[1], RX_MAX_OP_LEN, "#0%08Xh", uimm);
            *opCnt = 2;
            len = 7;
            break;
        case RX_GRP_DPU_CFD:
            // Convert from Double
            code = ((inst[3] & 4) >> 1) | (inst[3] & 1);
            reg1 = (inst[2] >> 4) & 15;
            reg2 = (inst[3] >> 4) & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_CFD_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "DR%d", reg1);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", reg2);
            *opCnt = 2;
            len = 4;
            break;
        case RX_GRP_DPU_CTD:
            // Convert to double
            code = inst[3] & 7;
            reg1 = inst[2] & 15;
            reg2 = (inst[3] >> 4) & 15;
            assert(RXD_DPU_CTD_opcodes[code] != NULL);
            snprintf(opc[0], RX_MAX_OP_LEN, "%s", RXD_DPU_CTD_opcodes[code]);
            snprintf(opc[1], RX_MAX_OP_LEN, "R%d", reg1);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", reg2);
            *opCnt = 2;
            len = 4;
            break;
        case RX_GRP_DPU_LD:
            // Load
            ld = inst[1] & 3;
            reg1 = (inst[2] >> 4) & 15;
            assert(ld != 3);
            reg2 = (inst[3 + RXD_getDspLen(ld)] >> 4) & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "DMOV.D");
            assert(ld != 3);
            if(ld == 0)
                snprintf(opc[1], RX_MAX_OP_LEN, "[R%d]", reg1);
            else
                snprintf(opc[1], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 3, ld) << 2, reg1);
            snprintf(opc[2], RX_MAX_OP_LEN, "DR%d", reg2);
            *opCnt = 2;
            len = 4 + RXD_getDspLen(ld);
            break;
        case RX_GRP_DPU_ST:
            // Store
            ld = inst[1] & 3;
            reg1 = (inst[2] >> 4) & 15;
            assert(ld != 3);
            reg2 = (inst[3 + RXD_getDspLen(ld)] >> 4) & 15;
            snprintf(opc[0], RX_MAX_OP_LEN, "DMOV.D");
            snprintf(opc[1], RX_MAX_OP_LEN, "DR%d", reg2);
            assert(ld != 3);
            if(ld == 0)
                snprintf(opc[2], RX_MAX_OP_LEN, "[R%d]", reg1);
            else
                snprintf(opc[2], RX_MAX_OP_LEN, "%d[R%d]", RXD_getDsp(inst + 3, ld) << 2, reg1);
            *opCnt = 2;
            len = 4 + RXD_getDspLen(ld);
            break;
        case RX_GRP_DPU_MVF:
            snprintf(opc[0], RX_MAX_OP_LEN, "MVFDR");
            *opCnt = 0;
            len = 3;
            break;
        default:
            assert(0);
            break;
        }

    return len;
}

//
// RXD_idec1
//     RX Part-1 Instruction Decoding
//

vint RXD_idec1(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt)
{
    vint len;
    vint b1_5;
    vint b2_4, b2_0;
    vint b0_7_6, b0_5, b0_5_4, b0_4, b0_3, b0_5_3, b0_2, b0_1, b0_1_0;

    b0_7_6 = inst[0] >> 6;
    if(b0_7_6 == 0)
        {
        // b0[7:6] == 00
        b0_5_3 = (inst[0] >> 3) & 7;
        if(b0_5_3 == 0)
            {
            // b0[5:3] == 0
            b0_2 = (inst[0] >> 2) & 1;
            if(b0_2 == 0)
                {
                // ZERO.OP2
                len = RXD_zero(inst, bytesLeft, version, opc, opCnt, RX_GRP_ZERO_OP2);
                }
            else
                {
                // b0[2] == 1
                b0_1 = (inst[0] >> 1) & 1;
                if(b0_1 == 0)
                    {
                    // CTL.IMM.A
                    len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_IMM_A);
                    }
                else
                    {
                    // b0[1] == 1
                    b1_5 = (inst[1] >> 5) & 1;
                    if(b1_5 == 0)
                        {
                        // AB.MI1(C)
                        len = RXD_IALU_rmem(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_MI1);
                        }
                    else
                        {
                        // b1[5] == 1
                        b2_4 = (inst[2] >> 4) & 1;
                        if(b2_4 == 0)
                            {
                            // AB.MI2(C)
                            len = RXD_IALU_rmem(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_MI2);
                            }
                        else
                            {
                            // b2[4] == 1
                            b2_0 = inst[2] & 1;
                            if(b2_0 == 0)
                                {
                                // DM.XCHG.MI
                                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_XCHG_MI);
                                }
                            else
                                {
                                // FPU.I2F.MI
                                len = RXD_fpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_FPU_I2F_MI);
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            // b0[5:3] <> 0
            b0_5 = (inst[0] >> 5) & 1;
            if(b0_5 == 0)
                {
                // CTL.IMM.S
                len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_IMM_S);
                }
            else
                {
                b0_4 = (inst[0] >> 4) & 1;
                if(b0_4 == 0)
                    {
                    // CTL.IMM.B
                    len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_IMM_B);
                    }
                else
                    {
                    b0_2 = (inst[0] >> 2) & 1;
                    if(b0_2 == 0)
                        {
                        // CTL.IMM.W
                        len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_IMM_W);
                        }
                    else
                        {
                        b0_1_0 = inst[0] & 3;
                        if(b0_1_0 != 3)
                            {
                            // DM.IMS
                            len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_IMS);
                            }
                        else
                            {
                            // CTL.ACR
                            len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_ACR);
                            }
                        }
                    }
                }
            }
        }
    else
        {
        // b0[7:6] == 10
        b0_3 = (inst[0] >> 3) & 1;
        b0_5_4 = (inst[0] >> 4) & 3;

        if(b0_5_4 == 3)
            {
            // LD.SOFF.U
            len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_SOFF_U);
            }
        else
            {
            if(b0_3 == 0)
                {
                // ST.SOFF
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_ST_SOFF);
                }
            else
                {
                // LD.SOFF.S
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_SOFF_S);
                }
            }
        }

    return len;
}

//
// RXD_idec2
//     RX Part-2 Instruction Decoding
//

vint RXD_idec2(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt)
{
    vint len;
    vint b2_1;
    vint b0_5_4, b0_3, b0_2, b0_1_0, b0_3_2;
    vint b1_1_0, b1_7_6, b1_5, b1_4, b1_3, b1_2;

    len = 0;
    b0_5_4 = (inst[0] >> 4) & 3;
    if(b0_5_4 != 3)
        {
        // b0[5:4] <> 11
        b0_3_2 = (inst[0] >> 2) & 3;
        b0_1_0 = inst[0] & 3;
        if(b0_3_2 != 3)
            {
            // b0[3:2] <> 11
            if(b0_1_0 != 3)
                {
                // DM.MM
                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_MM);
                }
            else
                {
                // ST.OFF
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_ST_OFF);
                }
            }
        else
            {
            // b0[3:2] == 11
            if(b0_1_0 != 3)
                {
                // LD.OFF.S
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_OFF_S);
                }
            else
                {
                // DM.RR
                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_RR);
                }
            }
        }
    else
        {
        // b0[5:4] == 11
        b0_3 = (inst[0] >> 3) & 1;
        b0_2 = (inst[0] >> 2) & 1;
        if(b0_3 == 0)
            {
            // b0[3] == 0
            if(b0_2 == 0)
                {
                // BM.IM1
                len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IM1);
                }
            else
                {
                // b0[2] == 1
                b1_3 = (inst[1] >> 3) & 1;
                if(b1_3 == 0)
                    {
                    // BM.IM1C
                    len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IM1C);
                    }
                else
                    {
                    // STK.SIM
                    len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_SIM);
                    }
                }
            }
        else
            {
            // b0[3] == 1
            if(b0_2 == 0)
                {
                // b0[2] == 0
                b1_1_0 = inst[1] & 3;
                if(b1_1_0 != 3)
                    {
                    // b1[1:0] <> 11
                    b0_1_0 = inst[0] & 3;
                    if(b0_1_0 != 3)
                        {
                        // DM.IM
                        len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_IM);
                        }
                    else
                        {
                        // DM.IR
                        len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_IR);
                        }
                    }
                else
                    {
                    // b1[1:0] == 11
                    b2_1 = (inst[2] >> 1) & 1;
                    if(b2_1 == 0)
                        {
                        // DPU.RTLI
                        len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RTLI);
                        }
                    else
                        {
                        // DPU.RTHI
                        len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RTHI);
                        }
                    }
                }
            else
                {
                // b0[2] == 1
                b1_7_6 = (inst[1] >> 6) & 3;
                if(b1_7_6 == 0)
                    {
                    // AB.LD2(C)
                    len = RXD_IALU_rmem(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_LD2);
                    }
                else if(b1_7_6 == 1)
                    {
                    // b1[7:6] == 01
                    b1_5 = (inst[1] >> 5) & 1;
                    b1_4 = (inst[1] >> 4) & 1;
                    b1_3 = (inst[1] >> 3) & 1;
                    b1_2 = (inst[1] >> 2) & 1;
                    if(b1_5 == 0)
                        {
                        // b1[5] == 0
                        if(b1_3 == 0)
                            {
                            // b1[3] == 0
                            if(b1_2 == 0)
                                {
                                // DM.XCHG.LD
                                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_XCHG_LD);
                                }
                            else
                                {
                                // FPU.I2F.LD
                                len = RXD_fpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_FPU_I2F_LD);
                                }
                            }
                        else
                            {
                            // b1[3] == 1
                            if(b1_4 == 0)
                                {
                                // DM.CMOV
                                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_CMOV);
                                }
                            else
                                {
                                // BM.RNG
                                len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_RNG);
                                }
                            }
                        }
                    else
                        {
                        // b1[5] == 1
                        if(b1_4 == 0)
                            {
                            // BM.REG
                            len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_REG);
                            }
                        else
                            {
                            // DPU.ST
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_ST);
                            }
                        }
                    }
                else
                    {
                    // FPU.LD
                    len = RXD_fpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_FPU_LD);
                    } 
                }
            }
        }

    return len;
}

//
// RXD_idec3
//     RX Part-3 Instruction Decoding
//

vint RXD_idec3(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt)
{
    vint len;
    vint b3_3;
    vint b2_3, b2_2;
    vint b1_7_6, b1_5, b1_4, b1_4_3, b1_7, b1_6, b1_5_4, b1_1_0;
    vint b0_5, b0_4_2, b0_4_3, b0_2_0, b0_2_1, b0_0, b0_2, b0_1;

    b0_5 = (inst[0] >> 5) & 1;
    if(b0_5 == 0)
        {
        // b0[5] == 0
        b0_4_2 = (inst[0] >> 2) & 7;
        if(b0_4_2 != 6 && b0_4_2 != 7)
            {
            // AB.LD1(C)
            len = RXD_IALU_rmem(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_LD1);
            }
        else
            {
            // LD.OFF.U
            len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_OFF_U);
            }
        }
    else
        {
        // b0[5] == 1
        b0_4_3 = (inst[0] >> 3) & 3;
        if(b0_4_3 == 0)
            {
            // b0[4:3] == 00
            b0_2_0 = inst[0] & 7;
            if(b0_2_0 == 7)
                {
                // CTL.ACA
                len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_ACA);
                }
            else
                {
                // AB.IR1(C)
                len = RXD_IALU_imm(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_IR1);
                }
            }
        else if(b0_4_3 == 1)
            {
            // b0[4:3] == 01
            b0_0 = inst[0] & 1;
            b0_2_1 = (inst[0] >> 1) & 3;
            if(b0_2_1 != 3)
                {
                // SH.IMM1
                len = RXD_shift(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_SH_IMM1);
                }
            else
                {
                if(b0_0 == 0)
                    {
                    // STK.MI
                    len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_MI);
                    }
                else
                    {
                    // STK.MO
                    len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_MO);
                    }
                }
            }
        else if(b0_4_3 == 2)
            {
            // b0[4:3] == 10
            b0_2 = (inst[0] >> 2) & 1;
            if(b0_2 == 0)
                {
                // AB.IR5
                len = RXD_IALU_imm(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_IR5);
                }
            else
                {
                // b0[2] == 1
                b1_7_6 = (inst[1] >> 6) & 3;
                if(b1_7_6 == 0)
                    {
                    // b1[7:6] == 00 -- AB.IR3(C)
                    len = RXD_IALU_imm(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_IR3);
                    }
                else if(b1_7_6 == 1)
                    {
                    // b1[7:6] == 01
                    b1_5 = (inst[1] >> 5) & 1;
                    b1_4 = (inst[1] >> 4) & 1;
                    if(b1_5 == 0)
                        {
                        // b1[5] == 0
                        if(b1_4 == 0)
                            {
                            // DM.IRS
                            len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_IRS);
                            }
                        else
                            {
                            // AB.IR2C
                            len = RXD_IALU_imm(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_IR2C);
                            }
                        }
                    else
                        {
                        // b1[5] == 1
                        if(b1_4 == 0)
                            {
                            // SYS.INT
                            len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_INT);
                            }
                        else
                            {
                            // SYS.IPL
                            len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_IPL);
                            }
                        }
                    }
                else if(b1_7_6 == 2)
                    {
                    // b1[7:6] == 10
                    b1_5 = (inst[1] >> 5) & 1;
                    if(b1_5 == 0)
                        {
                        // b1[5] == 0
                        b0_1 = (inst[0] >> 1) & 1;
                        if(b0_1 == 0)
                            {
                            // DPU.MVF
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_MVF);
                            }
                        else
                            {
                            // b0[1] == 1
                            b2_3 = (inst[2] >> 3) & 1;
                            b2_2 = (inst[2] >> 2) & 1;
                            b3_3 = (inst[3] >> 3) & 1;
                            if(b2_3 == 0)
                                {
                                // DPU.BA
                                len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_BA);
                                }
                            else
                                {
                                if(b2_2 == 0)
                                    {
                                    // DPU.CMP
                                    len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_CMP);
                                    }
                                else
                                    {
                                    if(b3_3 == 0)
                                        {
                                        // DPU.UAM
                                        len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_UAM);
                                        }
                                    else
                                        {
                                        // DPU.CFD
                                        len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_CFD);
                                        }
                                    }
                                }
                            }
                        }
                    else
                        {
                        // b1[5] == 1
                        b1_4_3 = (inst[1] >> 3) & 3;
                        if(b1_4_3 == 0)
                            {
                            // DPU.PUSHC
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_PUSHC);
                            }
                        else if(b1_4_3 == 1)
                            {
                            // DPU.POPC
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_POPC);
                            }
                        else if(b1_4_3 == 2)
                            {
                            // DPU.PUSH
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_PUSH);
                            }
                        else
                            {
                            // DPU.POP
                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_POP);
                            }
                        }
                    }
                else
                    assert(0);
                }
            }
        else
            {
            // b0[4:3] == 11
            b0_2 = (inst[0] >> 2) & 1;
            if(b0_2 == 0)
                {
                // BM.IR1
                len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IR1);
                }
            else
                {
                // b0[2] == 1
                b0_1 = (inst[0] >> 1) & 1;
                b0_0 = inst[0] & 1;
                if(b0_1 == 0)
                    {
                    // BM.IR1C
                    len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IR1C);
                    }
                else
                    {
                    if(b0_0 == 0)
                        {
                        b1_7 = (inst[1] >> 7) & 1;
                        b1_6 = (inst[1] >> 6) & 1;
                        b1_5_4 = (inst[1] >> 4) & 3;
                        if(b1_7 == 0)
                            {
                            if(b1_6 == 0)
                                {
                                // AB.REG1
                                len = RXD_IALU_reg(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_REG1);
                                }
                            else
                                {
                                // SH.CRY
                                len = RXD_shift(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_SH_CRY);
                                }
                            }
                        else
                            {
                            if(b1_6 == 0)
                                {
                                b1_5_4 = (inst[1] >> 4) & 3;
                                if(b1_5_4 == 3)
                                    {
                                    // STK.SO
                                    len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_SO);
                                    }
                                else
                                    {
                                    // STK.SI
                                    len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_SI);
                                    }
                                }
                            else
                                {
                                // STK.CTL
                                len = RXD_stack(inst, bytesLeft, version, opc, opCnt, RX_GRP_STK_CTL);
                                }
                            }
                        }
                    else
                        {
                        // b0[0] == 1
                        b1_5 = (inst[1] >> 5) & 1;
                        if(b1_5 == 0)
                            {
                            b1_7 = (inst[1] >> 7) & 1;
                            if(b1_7 == 0)
                                {
                                // CTL.REG
                                len = RXD_ctrl(inst, bytesLeft, version, opc, opCnt, RX_GRP_CTRL_REG);
                                }
                            else
                                {
                                b1_4 = (inst[1] >> 4) & 1;
                                if(b1_4 == 0)
                                    {
                                    b1_1_0 = inst[1] & 3;
                                    if(b1_1_0 == 3)
                                        {
                                        // ARY.B
                                        len = RXD_array(inst, bytesLeft, version, opc, opCnt, RX_GRP_ARRAY_BYTE);
                                        }
                                    else
                                        {
                                        // ARY.SZ
                                        len = RXD_array(inst, bytesLeft, version, opc, opCnt, RX_GRP_ARRAY_SZ);
                                        }
                                    }
                                else
                                    {
                                    // ZERO.OP1
                                    len = RXD_zero(inst, bytesLeft, version, opc, opCnt, RX_GRP_ZERO_OP1);
                                    }
                                }
                            }
                        else
                            {
                            // SYS.PSW
                            len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_PSW);
                            }
                        }
                    }
                }
            }
        }

    return len;
}

//
// RXD_idec4
//     RX Part-4 Instruction Decoding
//

vint RXD_idec4(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt)
{
    vint len;
    vint b0_1_0;
    vint b3_3, b3_2, b3_1;
    vint b2_3_0, b2_7_4, b2_7, b2_5;
    vint b1_7, b1_6, b1_6_5, b1_5, b1_4, b1_3, b1_2, b1_1, b1_1_0;

    b0_1_0 = inst[0] & 3;
    if(b0_1_0 == 0)
        {
        // b0[1:0] == 00
        b1_5 = (inst[1] >> 5) & 1;
        b1_4 = (inst[1] >> 4) & 1;
        b2_3_0 = (inst[2] & 15);
        if(b1_5 == 0)
            {
            // b1[5] == 0
            if(b1_4 == 0)
                {
                // DPU.LD
                len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_LD);
                }
            else
                {
                // DM.CRES
                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_CRES);
                }
            }
        else
            {
            // b1[5] == 1
            if(b2_3_0 != 15)
                {
                // BM.IM2
                len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IM2);
                }
            else
                {
                // BM.IM3
                len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IM3);
                }
            }
        }
    else if(b0_1_0 == 1)
        {
        // b0[1:0] == 01
        b1_7 = (inst[1] >> 7) & 1;
        if(b1_7 == 0)
            {
            b1_5 = (inst[1] >> 5) & 1;
            if(b1_5 == 0)
                {
                b1_4 = (inst[1] >> 4) & 1;
                if(b1_4 == 0)
                    {
                    // DSP.MUL
                    len = RXD_dsp(inst, bytesLeft, version, opc, opCnt, RX_GRP_DSP_MUL);
                    }
                else
                    {
                    b1_3 = (inst[1] >> 3) & 1;
                    if(b1_3 == 0)
                        {
                        // DSP.TAC
                        len = RXD_dsp(inst, bytesLeft, version, opc, opCnt, RX_GRP_DSP_TAC);
                        }
                    else
                        {
                        b1_2 = (inst[1] >> 2) & 1;
                        if(b1_2 == 0)
                            {
                            // DSP.RND
                            len = RXD_dsp(inst, bytesLeft, version, opc, opCnt, RX_GRP_DSP_RND);
                            }
                        else
                            {
                            // DSP.FAC
                            len = RXD_dsp(inst, bytesLeft, version, opc, opCnt, RX_GRP_DSP_FAC);
                            }
                        }
                    }
                }
            else
                {
                // b1[5] == 1
                b1_6 = (inst[1] >> 6) & 1;
                b1_4 = (inst[1] >> 4) & 1;
                if(b1_6 == 0)
                    {
                    // b1[6] == 0
                    if(b1_4 == 0)
                        {
                        // b1[4] == 0
                        b1_1_0 = inst[1] & 3;
                        if(b1_1_0 != 3)
                            {
                            b1_3 = (inst[1] >> 3) & 1;
                            if(b1_3 == 0)
                                {
                                // ST.SEQ
                                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_ST_SEQ_S);
                                }
                            else
                                {
                                // LD.SEQ.S
                                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_SEQ_S);
                                }
                            }
                        else
                            {
                            b1_3 = (inst[1] >> 3) & 1;
                            if(b1_3 == 0)
                                {
                                // DM.EX.ST
                                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_EX_ST);
                                }
                            else
                                {
                                // DM.EX.LD
                                len = RXD_dm(inst, bytesLeft, version, opc, opCnt, RX_GRP_DM_EX_LD);
                                }
                            }
                        }
                    else
                        {
                        // LS.SEQ.U
                        len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_SEQ_U);
                        }
                    }
                else
                    {
                    // b1[6] == 1
                    if(b1_4 == 0)
                        {
                        // b1[4] == 0
                        b1_3 = (inst[1] >> 3) & 1;
                        if(b1_3 == 0)
                            {
                            // SH.REG1
                            len = RXD_shift(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_SH_REG1);
                            }
                        else
                            {
                            // b1[3] == 1
                            b1_2 = (inst[1] >> 2) & 1;
                            if(b1_2 == 0)
                                {
                                // b1[2] == 0
                                b1_1 = (inst[1] >> 1) & 1;
                                if(b1_1 == 0)
                                    {
                                    // SYS.MTC
                                    len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_MTC);
                                    }
                                else
                                    {
                                    // SYS.MFC
                                    len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_MFC);
                                    }
                                }
                            else
                                {
                                // SH.IMM2
                                len = RXD_shift(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_SH_IMM2);
                                }
                            }
                        }
                    else
                        {
                        // b1[4] == 1
                        b1_1_0 = inst[1] & 3;
                        if(b1_1_0 == 0)
                            {
                            // AB.IR4(C)                            
                            len = RXD_IALU_imm(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_IR4);
                            }
                        else if(b1_1_0 == 1)
                            {
                            // b1[1:0] == 01
                            b3_2 = (inst[3] >> 2) & 1;
                            b3_1 = (inst[3] >> 1) & 1;
                            if(b3_2 == 0)
                                {
                                if(b3_1 == 0)
                                    {
                                    // DPU.RFL
                                    len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RFL);
                                    }
                                else
                                    {
                                    // DPU.RFH
                                    len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RFH);
                                    }
                                }
                            else
                                {
                                // DPU.RFC
                                len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RFC);
                                }
                            }
                        else if(b1_1_0 == 2)
                            {
                            // b1[1:0] == 10
                            b2_7 = (inst[2] >> 7) & 1;
                            b2_5 = (inst[2] >> 5) & 1;
                            if(b2_7 == 0)
                                {
                                // FPU.IMM
                                len = RXD_fpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_FPU_IMM);
                                }
                            else
                                {
                                // b2[7] == 1
                                if(b2_5 == 0)
                                    {
                                    // SYS.RBR
                                    len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_RBR);
                                    }
                                else
                                    {
                                    // SYS.RBI
                                    len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_RBI);
                                    }
                                }
                            }
                        else
                            {
                            // b1[1:0] == 11
                            b2_7 = (inst[2] >> 7) & 1;
                            if(b2_7 == 0)
                                {
                                // SYS.MTCI
                                len = RXD_system(inst, bytesLeft, version, opc, opCnt, RX_GRP_SYS_MTCI);
                                }
                            else
                                {
                                // b2[7] == 1
                                b3_3 = (inst[3] >> 3) & 1;
                                b3_2 = (inst[3] >> 2) & 1;
                                b3_1 = (inst[3] >> 1) & 1;
                                if(b3_3 == 0)
                                    {
                                    // b3[3] == 0
                                    if(b3_1 == 0)
                                        {
                                        // b3[1] == 0
                                        if(b3_2 == 0)
                                            {
                                            // DPU.RTL
                                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RTL);
                                            }
                                        else
                                            {
                                            // DPU.RTC
                                            len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RTC);
                                            }
                                        }
                                    else
                                        {
                                        // DPU.RTH
                                        len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_RTH);
                                        }
                                    }
                                else
                                    {
                                    // DPU.CTD
                                    len = RXD_dpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_DPU_CTD);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            b1_6_5 = (inst[1] >> 5) & 3;
            if(b1_6_5 != 3)
                {
                // SH.REG2
                len = RXD_shift(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_SH_REG2);
                }
            else
                {
                b2_7_4 = (inst[2] >> 4) & 15;
                if(b2_7_4 != 15)
                    {
                    // BM.IR2
                    len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IR2);
                    }
                else
                    {
                    // BM.IR3
                    len = RXD_bit(inst, bytesLeft, version, opc, opCnt, RX_GRP_BM_IR3);
                    }
                }
            }
        }
    else if(b0_1_0 == 2)
        {
        // b0[1:0] == 10
        b1_7 = (inst[1] >> 7) & 1;
        b1_6 = (inst[1] >> 6) & 1;
        if(b1_7 == 0)
            {
            // b1[7] == 0
            if(b1_6 == 0)
                {
                // ST.IDX
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_ST_IDX);
                }
            else
                {
                // LD.IDX.S
                len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_IDX_S);
                }
            }
        else
            {
            // LD.IDX.U
            len = RXD_ldst(inst, bytesLeft, version, opc, opCnt, RX_GRP_LD_IDX_U);
            }
        }
    else
        {
        // b0[1:0] == 11
        b1_7 = (inst[1] >> 7) & 1;
        if(b1_7 == 0)
            {
            // AB.REG2
            len = RXD_IALU_reg(inst, bytesLeft, version, opc, opCnt, RX_GRP_IA_AB_REG2);
            }
        else
            {
            // FPU.3REG
            len = RXD_fpu(inst, bytesLeft, version, opc, opCnt, RX_GRP_FPU_3REG);
            }
        }

    return len;
}

//
// RXD_idec
//     RX Top Level Instruction Decoding
//

vint RXD_idec(vuint8 *inst, vint bytesLeft, vint version, vchar **opc, vint *opCnt)
{
    vint b0_7, b0_6, b0_5_2, b0_1_0, b1_7_6;

    b0_7 = inst[0] >> 7;
    b0_6 = (inst[0] >> 6) & 1;
    b0_5_2 = (inst[0] >> 2) & 15;
    b0_1_0 = inst[0] & 3;

    if(b0_6 == 0)
        return RXD_idec1(inst, bytesLeft, version, opc, opCnt);
    else
        {
        if(b0_7 == 0)
            return RXD_idec3(inst, bytesLeft, version, opc, opCnt);
        else
            {
            if(b0_5_2 != 15)
                return RXD_idec2(inst, bytesLeft, version, opc, opCnt);
            else
                {
                if(b0_1_0 != 0)
                    return RXD_idec4(inst, bytesLeft, version, opc, opCnt);
                else
                    {
                    b1_7_6 = inst[1] >> 6;
                    if(b1_7_6 != 3)
                        return RXD_idec2(inst, bytesLeft, version, opc, opCnt);
                    else
                        return RXD_idec4(inst, bytesLeft, version, opc, opCnt);
                    }
                }
            }
        }
}

//
// main
//     Entry Point
//

int main(int argc, char *argv[])
{
    FILE *ifp;
    vuint8 *buf;
    vchar *opc[RX_MAX_OPC_OP_CNT];
    vint fileSize, i, j, k, opCnt, version, bi;

    if(argc != 3)
        {
        printf("Usage:: rxd.out <bin file> <ISA Version>\n");
        return 0;
        }

    if((ifp = fopen(argv[1], "rb")) == NULL)
        {
        printf("rxd:: Failed in opening input file\n");
        return 0;
        }

    version = atoi(argv[2]);
    assert(version == 1 || version == 2 || version == 3);

    for(i = 0; i < RX_MAX_OPC_OP_CNT; i++)
        opc[i] = cbuf + i * RX_MAX_OP_LEN;
    
    fseek(ifp, 0, SEEK_END);
    fileSize = ftell(ifp);
    fseek(ifp, 0, SEEK_SET);

    // Allocate 8 more bytes to take care of overread
    buf = malloc(fileSize + 8);
    assert(buf != NULL);

    fread(buf, 1, fileSize, ifp);

    bi = 0;
    while(bi < fileSize)
        {
        bi = bi + RXD_idec(buf + bi, fileSize - bi, version, opc, &opCnt);

        for(j = 0; j <= opCnt; j++)
            {
            if(j == 0)
                {
                if(opCnt == 0)
                    printf("    %s\n", opc[j]);
                else
                    {
                    printf("    %s", opc[j]);
                    for(k = 4 + strlen(opc[j]); k < 32; k++)
                        printf(" ");
                    }
                }
            else if(j != opCnt)
                printf("%s, ", opc[j]);
            else
                printf("%s\n", opc[j]);
            memset(opc[j], 0, RX_MAX_OP_LEN);
            }
        fflush(stdout);
        }

    fclose(ifp);
}
