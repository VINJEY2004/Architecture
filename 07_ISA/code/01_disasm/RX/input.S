;
; Copyright VINJEY Software Systems Pvt. Ltd, All Rights Reserved.
;
; All rights reserved. Any attempt or participation in deciphering, decoding,
; reverse engineering or in any way altering the source code is strictly
; prohibited, unless the prior written consent of VINJEY is obtained.
;
; Filename         :  input.S
; Description      :  Assembly Input Test code for disassembler validation
; Version Number   :  1.0
;
; To test with v1
; 1. Change VERSION value to 1
; 2. ccrx -fpu -nologo -dbl_size=8 -isa=rxv1 input.s
; 3. rx-elf-objcopy --dump-section P=input.bin input.obj
;
; To test with v2
; 1. Change VERSION value to 2
; 2. ccrx -fpu -nologo -dbl_size=8 -isa=rxv2 input.s
; 3. rx-elf-objcopy --dump-section P=input.bin input.obj
;
; To test with v3
; 1. Change VERSION value to 3
; 2. ccrx -fpu -dpfpu -nologo -dbl_size=8 -isa=rxv3 input.s
; 3. rx-elf-objcopy --dump-section P=input.bin input.obj
;

    .section P, CODE

VERSION                         .EQU                      3

    ;
    ; ABS
    ;

    ; ABS.1
    ABS                         R3

    ; ABS.2
    ABS                         R3, R4

    ;
    ; ADC
    ;

    ; ADC.1
    ADC                         #0FFFFFF80h, R3
    ADC                         #00000007Fh, R3
    ADC                         #0FFFF8000h, R3
    ADC                         #000007FFFh, R3
    ADC                         #0FF800000h, R3
    ADC                         #0007FFFFFh, R3
    ADC                         #080000000h, R3
    ADC                         #07FFFFFFFh, R3

    ; ADC.2
    ADC                         R1, R3

    ; ADC.3
    ADC                         [R1].L, R3
    ADC                         4[R1].L, R3
    ADC                         1024[R1].L, R3

    ;
    ; ADD
    ;

    ; ADD.1
    ADD                         #13, R3

    ; ADD.3A
    ADD                         R1, R3
    ADD                         [R1].UB, R3
    ADD                         1[R1].UB, R3
    ADD                         256[R1].UB, R3

    ; ADD.3B
    ADD                         [R1].B, R3
    ADD                         1[R1].B, R3
    ADD                         256[R1].B, R3

    ADD                         [R1].W, R3
    ADD                         2[R1].W, R3
    ADD                         512[R1].W, R3

    ADD                         [R1].UW, R3
    ADD                         2[R1].UW, R3
    ADD                         512[R1].UW, R3

    ADD                         [R1].L, R3
    ADD                         4[R1].L, R3
    ADD                         1024[R1].L, R3

    ; ADD.4
    ADD                         #0FFFFFF80h, R3, R4
    ADD                         #00000007Fh, R3, R4
    ADD                         #0FFFF8000h, R3, R4
    ADD                         #000007FFFh, R3, R4
    ADD                         #0FF800000h, R3, R4
    ADD                         #0007FFFFFh, R3, R4
    ADD                         #080000000h, R3, R4
    ADD                         #07FFFFFFFh, R3, R4

    ; ADD.5
    ADD                         R5, R3, R4

    ;
    ; AND
    ;

    ; AND.1
    AND                         #13, R3

    ; AND.2
    AND                         #0FFFFFF80h, R3
    AND                         #00000007Fh, R3
    AND                         #0FFFF8000h, R3
    AND                         #000007FFFh, R3
    AND                         #0FF800000h, R3
    AND                         #0007FFFFFh, R3
    AND                         #080000000h, R3
    AND                         #07FFFFFFFh, R3

    ; AND.3A
    AND                         R1, R3
    AND                         [R1].UB, R3
    AND                         1[R1].UB, R3
    AND                         256[R1].UB, R3

    ; AND.3B
    AND                         [R1].B, R3
    AND                         1[R1].B, R3
    AND                         256[R1].B, R3

    AND                         [R1].W, R3
    AND                         2[R1].W, R3
    AND                         512[R1].W, R3

    AND                         [R1].UW, R3
    AND                         2[R1].UW, R3
    AND                         512[R1].UW, R3

    AND                         [R1].L, R3
    AND                         4[R1].L, R3
    AND                         1024[R1].L, R3

    ; AND.4
    AND                         R5, R3, R4

    ;
    ; BCLR
    ;

    ; BCLR.1
    BCLR                        #5, [R1].B
    BCLR                        #5, 1[R1].B
    BCLR                        #5, 256[R1].B

    ; BCLR.2
    BCLR                        R3, [R1].B
    BCLR                        R3, 1[R1].B
    BCLR                        R3, 256[R1].B

    ; BCLR.3
    BCLR                        #5, R3

    ; BCLR.4
    BCLR                        R3, R4

.IF VERSION == 3
    ;
    ; BFMOV / BFMOVZ (v3)
    ;
 
    ; BFMOV.1
    BFMOV                       #0, #0, #1, R3, R4
    BFMOV                       #0, #0, #31, R3, R4
    BFMOV                       #1, #0, #31, R3, R4
    BFMOV                       #0, #1, #30, R3, R4
    BFMOV                       #31, #0, #1, R3, R4
    BFMOV                       #0, #30, #1, R3, R4
    BFMOV                       #31, #30, #1, R3, R4
    BFMOV                       #4, #8, #8, R3, R4
    BFMOV                       #8, #4, #8, R3, R4
    BFMOV                       #16, #0, #16, R3, R4
    BFMOV                       #0, #15, #16, R3, R4
    BFMOV                       #5, #20, #10, R14, R15
 
    ; BFMOVZ.1
    BFMOVZ                      #0, #0, #1, R3, R4
    BFMOVZ                      #0, #0, #31, R3, R4
    BFMOVZ                      #1, #0, #31, R3, R4
    BFMOVZ                      #0, #1, #30, R3, R4
    BFMOVZ                      #31, #0, #1, R3, R4
    BFMOVZ                      #0, #30, #1, R3, R4
    BFMOVZ                      #31, #30, #1, R3, R4
    BFMOVZ                      #4, #8, #8, R3, R4
    BFMOVZ                      #8, #4, #8, R3, R4
    BFMOVZ                      #16, #0, #16, R3, R4
    BFMOVZ                      #0, #15, #16, R3, R4
    BFMOVZ                      #5, #20, #10, R14, R15
.ENDIF

    ;
    ; BMCnd
    ;

    ; BMCnd.1
    BMEQ                        #5, [R1].B
    BMEQ                        #5, 1[R1].B
    BMEQ                        #5, 256[R1].B

    BMNE                        #5, [R1].B
    BMNE                        #5, 1[R1].B
    BMNE                        #5, 256[R1].B

    BMGEU                       #5, [R1].B
    BMGEU                       #5, 1[R1].B
    BMGEU                       #5, 256[R1].B

    BMLTU                       #5, [R1].B
    BMLTU                       #5, 1[R1].B
    BMLTU                       #5, 256[R1].B

    BMGTU                       #5, [R1].B
    BMGTU                       #5, 1[R1].B
    BMGTU                       #5, 256[R1].B

    BMLEU                       #5, [R1].B
    BMLEU                       #5, 1[R1].B
    BMLEU                       #5, 256[R1].B

    BMPZ                        #5, [R1].B
    BMPZ                        #5, 1[R1].B
    BMPZ                        #5, 256[R1].B

    BMN                         #5, [R1].B
    BMN                         #5, 1[R1].B
    BMN                         #5, 256[R1].B

    BMGE                        #5, [R1].B
    BMGE                        #5, 1[R1].B
    BMGE                        #5, 256[R1].B

    BMLT                        #5, [R1].B
    BMLT                        #5, 1[R1].B
    BMLT                        #5, 256[R1].B

    BMGT                        #5, [R1].B
    BMGT                        #5, 1[R1].B
    BMGT                        #5, 256[R1].B

    BMLE                        #5, [R1].B
    BMLE                        #5, 1[R1].B
    BMLE                        #5, 256[R1].B

    BMO                         #5, [R1].B
    BMO                         #5, 1[R1].B
    BMO                         #5, 256[R1].B

    BMNO                        #5, [R1].B
    BMNO                        #5, 1[R1].B
    BMNO                        #5, 256[R1].B

    ; BMCnd.2
    BMEQ                        #5, R1
    BMNE                        #5, R1
    BMGEU                       #5, R1
    BMLTU                       #5, R1
    BMGTU                       #5, R1
    BMLEU                       #5, R1
    BMPZ                        #5, R1
    BMN                         #5, R1
    BMGE                        #5, R1
    BMLT                        #5, R1
    BMGT                        #5, R1
    BMLE                        #5, R1
    BMO                         #5, R1
    BMNO                        #5, R1

    ;
    ; BNOT
    ;

    ; BNOT.1
    BNOT                        #5, [R1].B
    BNOT                        #5, 1[R1].B
    BNOT                        #5, 256[R1].B

    ; BNOT.2
    BNOT                        R3, [R1].B
    BNOT                        R3, 1[R1].B
    BNOT                        R3, 256[R1].B

    ; BNOT.3
    BNOT                        #5, R3

    ; BNOT.4
    BNOT                        R3, R4

    ;
    ; BRK
    ;

    ; BRK.1
    BRK

    ;
    ; BSET
    ;

    ; BSET.1
    BSET                        #5, [R1].B
    BSET                        #5, 1[R1].B
    BSET                        #5, 256[R1].B

    ; BSET.2
    BSET                        R3, [R1].B
    BSET                        R3, 1[R1].B
    BSET                        R3, 256[R1].B

    ; BSET.3
    BSET                        #5, R3

    ; BSET.4
    BSET                        R3, R4

    ;
    ; BSR
    ;

    ; BSR.3
    BSR.L                       R3

    ;
    ; BTST
    ;

    ; BTST.1
    BTST                        #5, [R1].B
    BTST                        #5, 1[R1].B
    BTST                        #5, 256[R1].B

    ; BTST.2
    BTST                        R3, [R1].B
    BTST                        R3, 1[R1].B
    BTST                        R3, 256[R1].B

    ; BTST.3
    BTST                        #5, R3

    ; BTST.4
    BTST                        R3, R4

    ;
    ; CLRPSW
    ;

    ; CLRPSW.1
    CLRPSW                      C
    CLRPSW                      Z
    CLRPSW                      S
    CLRPSW                      O
    CLRPSW                      I
    CLRPSW                      U

    ;
    ; CMP
    ;

    ; CMP.1
    CMP                         #13, R3

    ; CMP.2
    CMP                         #16, R3

    ; CMP.3
    CMP                         #0FFFFFF80h, R3
    CMP                         #0FFFF8000h, R3
    CMP                         #000007FFFh, R3
    CMP                         #0FF800000h, R3
    CMP                         #0007FFFFFh, R3
    CMP                         #080000000h, R3
    CMP                         #07FFFFFFFh, R3

    ; CMP.4A
    CMP                         R1, R3
    CMP                         [R1].UB, R3
    CMP                         1[R1].UB, R3
    CMP                         256[R1].UB, R3

    ; CMP.4B
    CMP                         [R1].B, R3
    CMP                         1[R1].B, R3
    CMP                         256[R1].B, R3

    CMP                         [R1].W, R3
    CMP                         2[R1].W, R3
    CMP                         512[R1].W, R3

    CMP                         [R1].UW, R3
    CMP                         2[R1].UW, R3
    CMP                         512[R1].UW, R3

    CMP                         [R1].L, R3
    CMP                         4[R1].L, R3
    CMP                         1024[R1].L, R3

    ;
    ; DIV
    ;

    ; DIV.1
    DIV                         #0FFFFFF80h, R3
    DIV                         #00000007Fh, R3
    DIV                         #0FFFF8000h, R3
    DIV                         #000007FFFh, R3
    DIV                         #0FF800000h, R3
    DIV                         #0007FFFFFh, R3
    DIV                         #080000000h, R3
    DIV                         #07FFFFFFFh, R3

    ; DIV.2A
    DIV                         R1, R3
    DIV                         [R1].UB, R3
    DIV                         1[R1].UB, R3
    DIV                         256[R1].UB, R3

    ; DIV.2B
    DIV                         [R1].B, R3
    DIV                         1[R1].B, R3
    DIV                         256[R1].B, R3

    DIV                         [R1].W, R3
    DIV                         2[R1].W, R3
    DIV                         512[R1].W, R3

    DIV                         [R1].UW, R3
    DIV                         2[R1].UW, R3
    DIV                         512[R1].UW, R3

    DIV                         [R1].L, R3
    DIV                         4[R1].L, R3
    DIV                         1024[R1].L, R3

    ;
    ; DIVU
    ;

    ; DIVU.1
    DIVU                        #0FFFFFF80h, R3
    DIVU                        #00000007Fh, R3
    DIVU                        #0FFFF8000h, R3
    DIVU                        #000007FFFh, R3
    DIVU                        #0FF800000h, R3
    DIVU                        #0007FFFFFh, R3
    DIVU                        #080000000h, R3
    DIVU                        #07FFFFFFFh, R3

    ; DIVU.2A
    DIVU                        R1, R3
    DIVU                        [R1].UB, R3
    DIVU                        1[R1].UB, R3
    DIVU                        256[R1].UB, R3

    ; DIVU.2B
    DIVU                        [R1].B, R3
    DIVU                        1[R1].B, R3
    DIVU                        256[R1].B, R3

    DIVU                        [R1].W, R3
    DIVU                        2[R1].W, R3
    DIVU                        512[R1].W, R3

    DIVU                        [R1].UW, R3
    DIVU                        2[R1].UW, R3
    DIVU                        512[R1].UW, R3

    DIVU                        [R1].L, R3
    DIVU                        4[R1].L, R3
    DIVU                        1024[R1].L, R3

.IF VERSION > 1
    ;
    ; EMACA
    ;

    ; EMACA.1
    EMACA                       R3, R4, A0
    EMACA                       R3, R4, A1

    ;
    ; EMSBA
    ;

    ; EMSBA.1
    EMSBA                       R3, R4, A0
    EMSBA                       R3, R4, A1
.ENDIF    

    ;
    ; EMUL
    ;

    ; EMUL.1
    EMUL                        #0FFFFFF80h, R3
    EMUL                        #00000007Fh, R3
    EMUL                        #0FFFF8000h, R3
    EMUL                        #000007FFFh, R3
    EMUL                        #0FF800000h, R3
    EMUL                        #0007FFFFFh, R3
    EMUL                        #080000000h, R3
    EMUL                        #07FFFFFFFh, R3

    ; EMUL.2A
    EMUL                        R1, R3
    EMUL                        [R1].UB, R3
    EMUL                        1[R1].UB, R3
    EMUL                        256[R1].UB, R3

    ; EMUL.2B
    EMUL                        [R1].B, R3
    EMUL                        1[R1].B, R3
    EMUL                        256[R1].B, R3

    EMUL                        [R1].W, R3
    EMUL                        2[R1].W, R3
    EMUL                        512[R1].W, R3

    EMUL                        [R1].UW, R3
    EMUL                        2[R1].UW, R3
    EMUL                        512[R1].UW, R3

    EMUL                        [R1].L, R3
    EMUL                        4[R1].L, R3
    EMUL                        1024[R1].L, R3

.IF VERSION > 1
    ;
    ; EMULA
    ;

    ; EMULA.1
    EMULA                       R3, R4, A0
    EMULA                       R3, R4, A1
.ENDIF

    ;
    ; EMULU
    ;

    ; EMULU.1
    EMULU                       #0FFFFFF80h, R3
    EMULU                       #00000007Fh, R3
    EMULU                       #0FFFF8000h, R3
    EMULU                       #000007FFFh, R3
    EMULU                       #0FF800000h, R3
    EMULU                       #0007FFFFFh, R3
    EMULU                       #080000000h, R3
    EMULU                       #07FFFFFFFh, R3

    ; EMULU.2A
    EMULU                       R1, R3
    EMULU                       [R1].UB, R3
    EMULU                       1[R1].UB, R3
    EMULU                       256[R1].UB, R3

    ; EMULU.2B
    EMULU                       [R1].B, R3
    EMULU                       1[R1].B, R3
    EMULU                       256[R1].B, R3

    EMULU                       [R1].W, R3
    EMULU                       2[R1].W, R3
    EMULU                       512[R1].W, R3

    EMULU                       [R1].UW, R3
    EMULU                       2[R1].UW, R3
    EMULU                       512[R1].UW, R3

    EMULU                       [R1].L, R3
    EMULU                       4[R1].L, R3
    EMULU                       1024[R1].L, R3

    ;
    ; FADD
    ;

    ; FADD.1
    FADD                        #03F800000h, R2

    ; FADD.2
    FADD                        R3, R2
    FADD                        [R3].L, R2
    FADD                        4[R3].L, R2
    FADD                        1024[R3].L, R2

.IF VERSION > 1
    ; FADD.3
    FADD                        R3, R4, R5
.ENDIF

    ;
    ; FCMP
    ;

    ; FCMP.1
    FCMP                        #03F800000h, R2

    ; FCMP.2
    FCMP                        R3, R2
    FCMP                        [R3].L, R2
    FCMP                        4[R3].L, R2
    FCMP                        1024[R3].L, R2

    ;
    ; FDIV
    ;

    ; FDIV.1
    FDIV                        #03F800000h, R2

    ; FDIV.2
    FDIV                        R3, R2
    FDIV                        [R3].L, R2
    FDIV                        4[R3].L, R2
    FDIV                        1024[R3].L, R2

    ;
    ; FMUL
    ;

    ; FMUL.1
    FMUL                        #03F800000h, R2

    ; FMUL.2
    FMUL                        R3, R2
    FMUL                        [R3].L, R2
    FMUL                        4[R3].L, R2
    FMUL                        1024[R3].L, R2

.IF VERSION > 1
    ; FMUL.3
    FMUL                        R3, R4, R5
.ENDIF

.IF VERSION > 1
    ;
    ; FSQRT
    ;

    ; FSQRT.1
    FSQRT                       R3, R2
    FSQRT                       [R3].L, R2
    FSQRT                       4[R3].L, R2
    FSQRT                       1024[R3].L, R2
.ENDIF

    ;
    ; FSUB
    ;

    ; FSUB.1
    FSUB                        #03F800000h, R2

    ; FSUB.2
    FSUB                        R3, R2
    FSUB                        [R3].L, R2
    FSUB                        4[R3].L, R2
    FSUB                        1024[R3].L, R2

.IF VERSION > 1
    ; FSUB.3
    FSUB                        R3, R4, R5
.ENDIF

    ;
    ; FTOI
    ;

    ; FTOI.1
    FTOI                        R3, R2
    FTOI                        [R3].L, R2
    FTOI                        4[R3].L, R2
    FTOI                        1024[R3].L, R2

.IF VERSION > 1
    ;
    ; FTOU
    ;

    ; FTOU.1
    FTOU                        R3, R2
    FTOU                        [R3].L, R2
    FTOU                        4[R3].L, R2
    FTOU                        1024[R3].L, R2
.ENDIF

    ;
    ; INT
    ;

    ; INT.1
    INT                         #5

    ;
    ; ITOF
    ;

    ; ITOF.1A
    ITOF                        R1, R3
    ITOF                        [R1].UB, R3
    ITOF                        1[R1].UB, R3
    ITOF                        256[R1].UB, R3

    ; ITOF.1B
    ITOF                        [R1].B, R3
    ITOF                        1[R1].B, R3
    ITOF                        256[R1].B, R3

    ITOF                        [R1].W, R3
    ITOF                        2[R1].W, R3
    ITOF                        512[R1].W, R3

    ITOF                        [R1].UW, R3
    ITOF                        2[R1].UW, R3
    ITOF                        512[R1].UW, R3

    ITOF                        [R1].L, R3
    ITOF                        4[R1].L, R3
    ITOF                        1024[R1].L, R3

    ;
    ; JMP
    ;

    ; JMP.1
    JMP                         R3

    ;
    ; JSR
    ;

    ; JSR.1
    JSR                         R3

.IF VERSION == 1
    ;
    ; MACHI
    ;

    ; MACHI.1
    MACHI                       R3, R4

    ;
    ; MACLO
    ;

    ; MACLO.1
    MACLO                       R3, R4
.ELSE
    ;
    ; MACHI
    ;

    ; MACHI.1
    MACHI                       R3, R4, A0
    MACHI                       R3, R4, A1

    ;
    ; MACLH
    ;

    ; MACLH.1
    MACLH                       R3, R4, A0
    MACLH                       R3, R4, A1

    ;
    ; MACLO
    ;

    ; MACLO.1
    MACLO                       R3, R4, A0
    MACLO                       R3, R4, A1
.ENDIF    

    ;
    ; MAX
    ;

    ; MAX.1
    MAX                         #0FFFFFF80h, R3
    MAX                         #00000007Fh, R3
    MAX                         #0FFFF8000h, R3
    MAX                         #000007FFFh, R3
    MAX                         #0FF800000h, R3
    MAX                         #0007FFFFFh, R3
    MAX                         #080000000h, R3
    MAX                         #07FFFFFFFh, R3

    ; MAX.2A
    MAX                         R1, R3
    MAX                         [R1].UB, R3
    MAX                         1[R1].UB, R3
    MAX                         256[R1].UB, R3

    ; MAX.2B
    MAX                         [R1].B, R3
    MAX                         1[R1].B, R3
    MAX                         256[R1].B, R3

    MAX                         [R1].W, R3
    MAX                         2[R1].W, R3
    MAX                         512[R1].W, R3

    MAX                         [R1].UW, R3
    MAX                         2[R1].UW, R3
    MAX                         512[R1].UW, R3

    MAX                         [R1].L, R3
    MAX                         4[R1].L, R3
    MAX                         1024[R1].L, R3

    ;
    ; MIN
    ;

    ; MIN.1
    MIN                         #0FFFFFF80h, R3
    MIN                         #00000007Fh, R3
    MIN                         #0FFFF8000h, R3
    MIN                         #000007FFFh, R3
    MIN                         #0FF800000h, R3
    MIN                         #0007FFFFFh, R3
    MIN                         #080000000h, R3
    MIN                         #07FFFFFFFh, R3

    ; MIN.2A
    MIN                         R1, R3
    MIN                         [R1].UB, R3
    MIN                         1[R1].UB, R3
    MIN                         256[R1].UB, R3

    ; MIN.2B
    MIN                         [R1].B, R3
    MIN                         1[R1].B, R3
    MIN                         256[R1].B, R3

    MIN                         [R1].W, R3
    MIN                         2[R1].W, R3
    MIN                         512[R1].W, R3

    MIN                         [R1].UW, R3
    MIN                         2[R1].UW, R3
    MIN                         512[R1].UW, R3

    MIN                         [R1].L, R3
    MIN                         4[R1].L, R3
    MIN                         1024[R1].L, R3

    ;
    ; MOV
    ;

    ; MOV.1
    MOV.B                       R3, [R4]
    MOV.B                       R3, 1[R4]
    MOV.W                       R3, [R4]
    MOV.W                       R3, 2[R4]
    MOV.L                       R3, [R4]
    MOV.L                       R3, 4[R4]

    ; MOV.2
    MOV.B                       [R4], R3
    MOV.B                       1[R4], R3
    MOV.W                       [R4], R3
    MOV.W                       2[R4], R3
    MOV.L                       [R4], R3
    MOV.L                       4[R4], R3

    ; MOV.3
    MOV.L                       #4, R3

    ; MOV.4
    MOV.B                       #255, 0[R3]
    MOV.W                       #255, 0[R3]
    MOV.L                       #255, 0[R3]

    MOV.B                       #255, 1[R3]
    MOV.W                       #255, 2[R3]
    MOV.L                       #255, 4[R3]

    ; MOV.5
    MOV.L                       #25, R3

    ; MOV.6
    MOV.L                       #0FFFFFF80h, R3
    MOV.L                       #0FFFF8000h, R3
    MOV.L                       #000007FFFh, R3
    MOV.L                       #0FF800000h, R3
    MOV.L                       #0007FFFFFh, R3
    MOV.L                       #080000000h, R3
    MOV.L                       #07FFFFFFFh, R3

    ; MOV.7
    MOV.B                       R3, R4
    MOV.W                       R3, R4
    MOV.L                       R3, R4

    ; MOV.8
    MOV.B                       #000000001h, [R9]
    MOV.B                       #000000001h, 1[R9]
    MOV.B                       #000000001h, 256[R9]

    MOV.W                       #00000007Fh, [R9]
    MOV.W                       #0FFFFFF80h, [R9]
    MOV.W                       #0FFFF8000h, [R9]
    MOV.W                       #000007FFFh, [R9]

    MOV.W                       #00000007Fh, 2[R9]
    MOV.W                       #0FFFFFF80h, 2[R9]
    MOV.W                       #0FFFF8000h, 2[R9]
    MOV.W                       #000007FFFh, 2[R9]

    MOV.W                       #00000007Fh, 512[R9]
    MOV.W                       #0FFFFFF80h, 512[R9]
    MOV.W                       #0FFFF8000h, 512[R9]
    MOV.W                       #000007FFFh, 512[R9]

    MOV.L                       #00000007Fh, [R9]
    MOV.L                       #0FFFFFF80h, [R9]
    MOV.L                       #0FFFF8000h, [R9]
    MOV.L                       #000007FFFh, [R9]
    MOV.L                       #0FF800000h, [R9]
    MOV.L                       #0007FFFFFh, [R9]
    MOV.L                       #080000000h, [R9]
    MOV.L                       #07FFFFFFFh, [R9]

    MOV.L                       #00000007Fh, 4[R9]
    MOV.L                       #0FFFFFF80h, 4[R9]
    MOV.L                       #0FFFF8000h, 4[R9]
    MOV.L                       #000007FFFh, 4[R9]
    MOV.L                       #0FF800000h, 4[R9]
    MOV.L                       #0007FFFFFh, 4[R9]
    MOV.L                       #080000000h, 4[R9]
    MOV.L                       #07FFFFFFFh, 4[R9]

    MOV.L                       #00000007Fh, 1024[R9]
    MOV.L                       #0FFFFFF80h, 1024[R9]
    MOV.L                       #0FFFF8000h, 1024[R9]
    MOV.L                       #000007FFFh, 1024[R9]
    MOV.L                       #0FF800000h, 1024[R9]
    MOV.L                       #0007FFFFFh, 1024[R9]
    MOV.L                       #080000000h, 1024[R9]
    MOV.L                       #07FFFFFFFh, 1024[R9]

    ; MOV.9
    MOV.B                       [R8], R9
    MOV.B                       1[R8], R9
    MOV.B                       256[R8], R9

    MOV.W                       [R8], R9
    MOV.W                       2[R8], R9
    MOV.W                       512[R8], R9

    MOV.L                       [R8], R9
    MOV.L                       4[R8], R9
    MOV.L                       1024[R8], R9

    ; MOV.10
    MOV.B                       [R8, R10], R9
    MOV.W                       [R8, R10], R9
    MOV.L                       [R8, R10], R9

    ; MOV.11
    MOV.B                       R9, [R8]
    MOV.B                       R9, 1[R8]
    MOV.B                       R9, 256[R8]

    MOV.W                       R9, [R8]
    MOV.W                       R9, 2[R8]
    MOV.W                       R9, 512[R8]

    MOV.L                       R9, [R8]
    MOV.L                       R9, 4[R8]
    MOV.L                       R9, 1024[R8]

    ; MOV.12
    MOV.B                       R9, [R8, R10]
    MOV.W                       R9, [R8, R10]
    MOV.L                       R9, [R8, R10]

    ; MOV.13
    MOV.B                       [R9], [R8]
    MOV.B                       [R9], 1[R8]
    MOV.B                       [R9], 256[R8]

    MOV.W                       [R9], [R8]
    MOV.W                       [R9], 2[R8]
    MOV.W                       [R9], 512[R8]

    MOV.L                       [R9], [R8]
    MOV.L                       [R9], 4[R8]
    MOV.L                       [R9], 1024[R8]

    MOV.B                       1[R9], [R8]
    MOV.B                       1[R9], 1[R8]
    MOV.B                       1[R9], 256[R8]

    MOV.W                       2[R9], [R8]
    MOV.W                       2[R9], 2[R8]
    MOV.W                       2[R9], 512[R8]

    MOV.L                       4[R9], [R8]
    MOV.L                       4[R9], 4[R8]
    MOV.L                       4[R9], 1024[R8]

    MOV.B                       256[R9], [R8]
    MOV.B                       256[R9], 1[R8]
    MOV.B                       256[R9], 256[R8]

    MOV.W                       512[R9], [R8]
    MOV.W                       512[R9], 2[R8]
    MOV.W                       512[R9], 512[R8]

    MOV.L                       1024[R9], [R8]
    MOV.L                       1024[R9], 4[R8]
    MOV.L                       1024[R9], 1024[R8]

    ; MOV.14
    MOV.B                       R3, [R4+]
    MOV.W                       R3, [R4+]
    MOV.L                       R3, [R4+]

    MOV.B                       R3, [-R4]
    MOV.W                       R3, [-R4]
    MOV.L                       R3, [-R4]

    ; MOV.15
    MOV.B                       [R4+], R3
    MOV.W                       [R4+], R3
    MOV.L                       [R4+], R3

    MOV.B                       [-R4], R3
    MOV.W                       [-R4], R3
    MOV.L                       [-R4], R3

.IF VERSION > 1
    ; MOVCO.1
    MOVCO                       R3, [R4]

    ; MOVLI.1
    MOVLI                       [R4], R3
.ENDIF

    ;
    ; MOVU
    ;

    ; MOVU.1
    MOVU.B                      [R3], R4
    MOVU.B                      1[R3], R4

    MOVU.W                      [R3], R4
    MOVU.W                      2[R3], R4

    ; MOVU.2
    MOVU.B                      R8, R4
    MOVU.W                      R8, R4

    MOVU.B                      [R8], R4
    MOVU.B                      1[R8], R4
    MOVU.B                      256[R8], R4

    MOVU.W                      [R8], R4
    MOVU.W                      2[R8], R4
    MOVU.W                      512[R8], R4

    ; MOVU.3
    MOVU.B                      [R8, R9], R4
    MOVU.W                      [R8, R9], R4

    ; MOVU.4
    MOVU.B                      [R8+], R4
    MOVU.B                      [-R8], R4
    MOVU.W                      [R8+], R4
    MOVU.W                      [-R8], R4

.IF VERSION > 1
    ;
    ; MSBHI
    ;

    ; MSBHI.1
    MSBHI                       R3, R4, A0
    MSBHI                       R3, R4, A1

    ;
    ; MSBLH
    ;

    ; MSBLH.1
    MSBLH                       R3, R4, A0
    MSBLH                       R3, R4, A1

    ;
    ; MSBLO
    ;

    ; MSBLO.1
    MSBLO                       R3, R4, A0
    MSBLO                       R3, R4, A1
.ENDIF    

    ;
    ; MUL
    ;

    ; MUL.1
    MUL                         #13, R3

    ; MUL.2
    MUL                         #0FFFFFF80h, R3
    MUL                         #00000007Fh, R3
    MUL                         #0FFFF8000h, R3
    MUL                         #000007FFFh, R3
    MUL                         #0FF800000h, R3
    MUL                         #0007FFFFFh, R3
    MUL                         #080000000h, R3
    MUL                         #07FFFFFFFh, R3

    ; MUL.3A
    MUL                         R1, R3
    MUL                         [R1].UB, R3
    MUL                         1[R1].UB, R3
    MUL                         256[R1].UB, R3

    ; MUL.3B
    MUL                         [R1].B, R3
    MUL                         1[R1].B, R3
    MUL                         256[R1].B, R3

    MUL                         [R1].W, R3
    MUL                         2[R1].W, R3
    MUL                         512[R1].W, R3

    MUL                         [R1].UW, R3
    MUL                         2[R1].UW, R3
    MUL                         512[R1].UW, R3

    MUL                         [R1].L, R3
    MUL                         4[R1].L, R3
    MUL                         1024[R1].L, R3

    ; MUL.4
    MUL                         R5, R3, R4

.IF VERSION == 1
    ;
    ; MULHI
    ;

    ; MULHI.1
    MULHI                       R3, R4

    ;
    ; MULLO
    ;

    ; MULLO.1
    MULLO                       R3, R4

    ;
    ; MVFACHI
    ;

    ; MVFACHI.1
    MVFACHI                     R3

    ;
    ; MVFACMI
    ;

    ; MVFACMI.1
    MVFACMI                     R3
.ELSE
    ;
    ; MULHI
    ;

    ; MULHI.1
    MULHI                       R3, R4, A0
    MULHI                       R3, R4, A1

    ;
    ; MULLH
    ;

    ; MULLH.1
    MULLH                       R3, R4, A0
    MULLH                       R3, R4, A1

    ;
    ; MULLO
    ;

    ; MULLO.1
    MULLO                       R3, R4, A0
    MULLO                       R3, R4, A1

    ;
    ; MVFACGU
    ;

    ; MVFACGU.1
    MVFACGU                     #0, A0, R3
    MVFACGU                     #1, A0, R3
    MVFACGU                     #2, A0, R3
    MVFACGU                     #0, A1, R3
    MVFACGU                     #1, A1, R3
    MVFACGU                     #2, A1, R3

    ;
    ; MVFACHI
    ;

    ; MVFACHI.1
    MVFACHI                     #0, A0, R3
    MVFACHI                     #1, A0, R3
    MVFACHI                     #2, A0, R3
    MVFACHI                     #0, A1, R3
    MVFACHI                     #1, A1, R3
    MVFACHI                     #2, A1, R3

    ;
    ; MVFACLO
    ;

    ; MVFACLO.1
    MVFACLO                     #0, A0, R3
    MVFACLO                     #1, A0, R3
    MVFACLO                     #2, A0, R3
    MVFACLO                     #0, A1, R3
    MVFACLO                     #1, A1, R3
    MVFACLO                     #2, A1, R3

    ;
    ; MVFACMI
    ;

    ; MVFACMI.1
    MVFACMI                     #0, A0, R3
    MVFACMI                     #1, A0, R3
    MVFACMI                     #2, A0, R3
    MVFACMI                     #0, A1, R3
    MVFACMI                     #1, A1, R3
    MVFACMI                     #2, A1, R3
.ENDIF    

    ;
    ; MVFC
    ;

    ; MVFC.1
    MVFC                        PSW, R3
    MVFC                        PC, R3
    MVFC                        USP, R3
    MVFC                        FPSW, R3
    MVFC                        BPSW, R3
    MVFC                        BPC, R3
    MVFC                        ISP, R3
    MVFC                        FINTV, R3
    MVFC                        INTB, R3

.IF VERSION == 1
    ;
    ; MVTACHI
    ;

    ; MVTACHI.1
    MVTACHI                     R3

    ;
    ; MVTACLO
    ;

    ; MVTACLO.1
    MVTACLO                     R3
.ELSE
    ;
    ; MVTACGU
    ;

    ; MVTACGU.1
    MVTACGU                     R3, A0
    MVTACGU                     R3, A1

    ;
    ; MVTACHI
    ;

    ; MVTACHI.1
    MVTACHI                     R3, A0
    MVTACHI                     R3, A1

    ;
    ; MVTACLO
    ;

    ; MVTACLO.1
    MVTACLO                     R3, A0
    MVTACLO                     R3, A1
.ENDIF    

    ;
    ; MVTC
    ;

    ; MVTC.1
    MVTC                        #0FFFFFF80h, PSW
    MVTC                        #00000007Fh, PSW
    MVTC                        #0FFFF8000h, PSW
    MVTC                        #000007FFFh, PSW
    MVTC                        #0FF800000h, PSW
    MVTC                        #0007FFFFFh, PSW
    MVTC                        #080000000h, PSW
    MVTC                        #07FFFFFFFh, PSW

    MVTC                        #0FFFFFF80h, USP
    MVTC                        #00000007Fh, USP
    MVTC                        #0FFFF8000h, USP
    MVTC                        #000007FFFh, USP
    MVTC                        #0FF800000h, USP
    MVTC                        #0007FFFFFh, USP
    MVTC                        #080000000h, USP
    MVTC                        #07FFFFFFFh, USP

    MVTC                        #0FFFFFF80h, FPSW
    MVTC                        #00000007Fh, FPSW
    MVTC                        #0FFFF8000h, FPSW
    MVTC                        #000007FFFh, FPSW
    MVTC                        #0FF800000h, FPSW
    MVTC                        #0007FFFFFh, FPSW
    MVTC                        #080000000h, FPSW
    MVTC                        #07FFFFFFFh, FPSW

    MVTC                        #0FFFFFF80h, BPSW
    MVTC                        #00000007Fh, BPSW
    MVTC                        #0FFFF8000h, BPSW
    MVTC                        #000007FFFh, BPSW
    MVTC                        #0FF800000h, BPSW
    MVTC                        #0007FFFFFh, BPSW
    MVTC                        #080000000h, BPSW
    MVTC                        #07FFFFFFFh, BPSW

    MVTC                        #0FFFFFF80h, BPC
    MVTC                        #00000007Fh, BPC
    MVTC                        #0FFFF8000h, BPC
    MVTC                        #000007FFFh, BPC
    MVTC                        #0FF800000h, BPC
    MVTC                        #0007FFFFFh, BPC
    MVTC                        #080000000h, BPC
    MVTC                        #07FFFFFFFh, BPC

    MVTC                        #0FFFFFF80h, ISP
    MVTC                        #00000007Fh, ISP
    MVTC                        #0FFFF8000h, ISP
    MVTC                        #000007FFFh, ISP
    MVTC                        #0FF800000h, ISP
    MVTC                        #0007FFFFFh, ISP
    MVTC                        #080000000h, ISP
    MVTC                        #07FFFFFFFh, ISP

    MVTC                        #0FFFFFF80h, FINTV
    MVTC                        #00000007Fh, FINTV
    MVTC                        #0FFFF8000h, FINTV
    MVTC                        #000007FFFh, FINTV
    MVTC                        #0FF800000h, FINTV
    MVTC                        #0007FFFFFh, FINTV
    MVTC                        #080000000h, FINTV
    MVTC                        #07FFFFFFFh, FINTV

    MVTC                        #0FFFFFF80h, INTB
    MVTC                        #00000007Fh, INTB
    MVTC                        #0FFFF8000h, INTB
    MVTC                        #000007FFFh, INTB
    MVTC                        #0FF800000h, INTB
    MVTC                        #0007FFFFFh, INTB
    MVTC                        #080000000h, INTB
    MVTC                        #07FFFFFFFh, INTB

    ; MVTC.2
    MVTC                        R3, PSW
    MVTC                        R3, USP
    MVTC                        R3, FPSW
    MVTC                        R3, BPSW
    MVTC                        R3, BPC
    MVTC                        R3, ISP
    MVTC                        R3, FINTV
    MVTC                        R3, INTB

    ;
    ; MVTIPL
    ;

    ; MVTIPL.1
    MVTIPL                      #0
    MVTIPL                      #1
    MVTIPL                      #2
    MVTIPL                      #3
    MVTIPL                      #4
    MVTIPL                      #5
    MVTIPL                      #6
    MVTIPL                      #7
    MVTIPL                      #8
    MVTIPL                      #9
    MVTIPL                      #10
    MVTIPL                      #11
    MVTIPL                      #12
    MVTIPL                      #13
    MVTIPL                      #14
    MVTIPL                      #15
    
    ;
    ; NEG
    ;

    ; NEG.1
    NEG                         R3

    ; NEG.2
    NEG                         R3, R4

    ;
    ; NOP
    ;

    ; NOP.1
    NOP

    ;
    ; NOT
    ;

    ; NOT.1
    NOT                         R3

    ; NOT.2
    NOT                         R3, R4

    ;
    ; OR 
    ;

    ; OR .1
    OR                          #13, R3

    ; OR .2
    OR                          #0FFFFFF80h, R3
    OR                          #00000007Fh, R3
    OR                          #0FFFF8000h, R3
    OR                          #000007FFFh, R3
    OR                          #0FF800000h, R3
    OR                          #0007FFFFFh, R3
    OR                          #080000000h, R3
    OR                          #07FFFFFFFh, R3

    ; OR .3A
    OR                          R1, R3
    OR                          [R1].UB, R3
    OR                          1[R1].UB, R3
    OR                          256[R1].UB, R3

    ; OR .3B
    OR                          [R1].B, R3
    OR                          1[R1].B, R3
    OR                          256[R1].B, R3

    OR                          [R1].W, R3
    OR                          2[R1].W, R3
    OR                          512[R1].W, R3

    OR                          [R1].UW, R3
    OR                          2[R1].UW, R3
    OR                          512[R1].UW, R3

    OR                          [R1].L, R3
    OR                          4[R1].L, R3
    OR                          1024[R1].L, R3

    ; OR .4
    OR                          R5, R3, R4

    ;
    ; POP
    ; 

    ; POP.1
    POP                         R3

    ;
    ; POPC
    ;

    ; POPC.1
    POPC                        PSW
    POPC                        USP
    POPC                        FPSW
    POPC                        BPSW
    POPC                        BPC
    POPC                        ISP
    POPC                        FINTV
    POPC                        INTB

    ;
    ; POPM
    ;

    ; POPM.1
    POPM                        R4-R8

    ;
    ; PUSH
    ; 

    ; PUSH.1
    PUSH.B                      R3
    PUSH.W                      R3
    PUSH.L                      R3

    ; PUSH.2
    PUSH.B                      [R3]
    PUSH.W                      [R3]
    PUSH.L                      [R3]

    PUSH.B                      1[R3]
    PUSH.W                      2[R3]
    PUSH.L                      4[R3]

    PUSH.B                      256[R3]
    PUSH.W                      512[R3]
    PUSH.L                      1024[R3]

    ;
    ; PUSHC
    ;

    ; PUSHC.1
    PUSHC                       PSW
    PUSHC                       PC
    PUSHC                       USP
    PUSHC                       FPSW
    PUSHC                       BPSW
    PUSHC                       BPC
    PUSHC                       ISP
    PUSHC                       FINTV
    PUSHC                       INTB

    ;
    ; PUSHM
    ;

    ; PUSHM.1
    PUSHM                       R4-R8

.IF VERSION > 1
    ;
    ; RACL
    ;

    ; RACL.1
    RACL                        #1, A0
    RACL                        #2, A0

    RACL                        #1, A1
    RACL                        #2, A1
.ENDIF    

    ;
    ; RACW
    ;

.IF VERSION == 1
    ; RACW.1
    RACW                        #1
    RACW                        #2
.ELSE
    ; RACW.1
    RACW                        #1, A0
    RACW                        #2, A0

    RACW                        #1, A1
    RACW                        #2, A1
.ENDIF    

.IF VERSION > 1
    ;
    ; RDACL
    ;

    ; RDACL.1
    RDACL                       #1, A0
    RDACL                       #2, A0

    RDACL                       #1, A1
    RDACL                       #2, A1

    ;
    ; RDACW
    ;

    ; RDACW.1
    RDACW                       #1, A0
    RDACW                       #2, A0

    RDACW                       #1, A1
    RDACW                       #2, A1
.ENDIF    

    ;
    ; REVL
    ;

    ; REVL.1
    REVL                        R4, R3

    ; REVW.1
    REVW                        R4, R3

    ;
    ; RMPA
    ;

    ; RMPA.1
    RMPA.B
    RMPA.W
    RMPA.L

    ;
    ; ROLC
    ;

    ; ROLC.1
    ROLC                        R3

    ;
    ; RORC
    ;

    ; RORC.1
    RORC                        R3

    ;
    ; ROTL
    ;

    ; ROTL.1
    ROTL                        #3, R3

    ; ROTL.2
    ROTL                        R4, R3

    ;
    ; ROTR
    ;

    ; ROTR.1
    ROTR                        #3, R3

    ; ROTR.2
    ROTR                        R4, R3

    ;
    ; ROUND
    ;

    ; ROUND.1
    ROUND                       R3, R2
    ROUND                       [R3].L, R2
    ROUND                       4[R3].L, R2
    ROUND                       1024[R3].L, R2

    ;
    ; RTE
    ;

    ; RTE.1
    RTE

    ;
    ; RTFI
    ;

    ; RTFI.1
    RTFI

    ;
    ; RTS
    ;

    ; RTS.1
    RTS

    ;
    ; RTSD
    ;

    ; RTSD.1
    RTSD                        #12

    ; RTSD.2
    RTSD                        #12, R3-R5

    ;
    ; SAT
    ;

    ; SAT.1
    SAT                         R5

    ;
    ; SATR
    ;

    ; SATR.1
    SATR

    ;
    ; SBB
    ;

    ; SBB.1
    SBB                         R1, R3

    ; SBB.2
    SBB                         [R1].L, R3
    SBB                         4[R1].L, R3
    SBB                         1024[R1].L, R3

    ;
    ; SCCnd
    ;

    ; SCCnd.1
    SCEQ.L                      R3
    SCNE.L                      R3
    SCGEU.L                     R3
    SCLTU.L                     R3
    SCGTU.L                     R3
    SCLEU.L                     R3
    SCPZ.L                      R3
    SCN.L                       R3
    SCGE.L                      R3
    SCLT.L                      R3
    SCGT.L                      R3
    SCLE.L                      R3
    SCO.L                       R3
    SCNO.L                      R3

    SCEQ.B                      [R3]
    SCNE.B                      [R3]
    SCGEU.B                     [R3]
    SCLTU.B                     [R3]
    SCGTU.B                     [R3]
    SCLEU.B                     [R3]
    SCPZ.B                      [R3]
    SCN.B                       [R3]
    SCGE.B                      [R3]
    SCLT.B                      [R3]
    SCGT.B                      [R3]
    SCLE.B                      [R3]
    SCO.B                       [R3]
    SCNO.B                      [R3]

    SCEQ.B                      1[R3]
    SCNE.B                      1[R3]
    SCGEU.B                     1[R3]
    SCLTU.B                     1[R3]
    SCGTU.B                     1[R3]
    SCLEU.B                     1[R3]
    SCPZ.B                      1[R3]
    SCN.B                       1[R3]
    SCGE.B                      1[R3]
    SCLT.B                      1[R3]
    SCGT.B                      1[R3]
    SCLE.B                      1[R3]
    SCO.B                       1[R3]
    SCNO.B                      1[R3]

    SCEQ.B                      256[R3]
    SCNE.B                      256[R3]
    SCGEU.B                     256[R3]
    SCLTU.B                     256[R3]
    SCGTU.B                     256[R3]
    SCLEU.B                     256[R3]
    SCPZ.B                      256[R3]
    SCN.B                       256[R3]
    SCGE.B                      256[R3]
    SCLT.B                      256[R3]
    SCGT.B                      256[R3]
    SCLE.B                      256[R3]
    SCO.B                       256[R3]
    SCNO.B                      256[R3]

    SCEQ.W                      [R3]
    SCNE.W                      [R3]
    SCGEU.W                     [R3]
    SCLTU.W                     [R3]
    SCGTU.W                     [R3]
    SCLEU.W                     [R3]
    SCPZ.W                      [R3]
    SCN.W                       [R3]
    SCGE.W                      [R3]
    SCLT.W                      [R3]
    SCGT.W                      [R3]
    SCLE.W                      [R3]
    SCO.W                       [R3]
    SCNO.W                      [R3]

    SCEQ.W                      2[R3]
    SCNE.W                      2[R3]
    SCGEU.W                     2[R3]
    SCLTU.W                     2[R3]
    SCGTU.W                     2[R3]
    SCLEU.W                     2[R3]
    SCPZ.W                      2[R3]
    SCN.W                       2[R3]
    SCGE.W                      2[R3]
    SCLT.W                      2[R3]
    SCGT.W                      2[R3]
    SCLE.W                      2[R3]
    SCO.W                       2[R3]
    SCNO.W                      2[R3]

    SCEQ.W                      512[R3]
    SCNE.W                      512[R3]
    SCGEU.W                     512[R3]
    SCLTU.W                     512[R3]
    SCGTU.W                     512[R3]
    SCLEU.W                     512[R3]
    SCPZ.W                      512[R3]
    SCN.W                       512[R3]
    SCGE.W                      512[R3]
    SCLT.W                      512[R3]
    SCGT.W                      512[R3]
    SCLE.W                      512[R3]
    SCO.W                       512[R3]
    SCNO.W                      512[R3]

    SCEQ.L                      [R3]
    SCNE.L                      [R3]
    SCGEU.L                     [R3]
    SCLTU.L                     [R3]
    SCGTU.L                     [R3]
    SCLEU.L                     [R3]
    SCPZ.L                      [R3]
    SCN.L                       [R3]
    SCGE.L                      [R3]
    SCLT.L                      [R3]
    SCGT.L                      [R3]
    SCLE.L                      [R3]
    SCO.L                       [R3]
    SCNO.L                      [R3]

    SCEQ.L                      4[R3]
    SCNE.L                      4[R3]
    SCGEU.L                     4[R3]
    SCLTU.L                     4[R3]
    SCGTU.L                     4[R3]
    SCLEU.L                     4[R3]
    SCPZ.L                      4[R3]
    SCN.L                       4[R3]
    SCGE.L                      4[R3]
    SCLT.L                      4[R3]
    SCGT.L                      4[R3]
    SCLE.L                      4[R3]
    SCO.L                       4[R3]
    SCNO.L                      4[R3]

    SCEQ.L                      1024[R3]
    SCNE.L                      1024[R3]
    SCGEU.L                     1024[R3]
    SCLTU.L                     1024[R3]
    SCGTU.L                     1024[R3]
    SCLEU.L                     1024[R3]
    SCPZ.L                      1024[R3]
    SCN.L                       1024[R3]
    SCGE.L                      1024[R3]
    SCLT.L                      1024[R3]
    SCGT.L                      1024[R3]
    SCLE.L                      1024[R3]
    SCO.L                       1024[R3]
    SCNO.L                      1024[R3]

    ;
    ; SCMPU
    ;

    ; SCMPU.1
    SCMPU

    ;
    ; SETPSW
    ;

    ; SETPSW.1
    SETPSW                      C
    SETPSW                      Z
    SETPSW                      S
    SETPSW                      O
    SETPSW                      I
    SETPSW                      U

    ;
    ; SHAR
    ;

    ; SHAR.1
    SHAR                        #13, R3

    ; SHAR.2
    SHAR                        R3, R4

    ; SHAR.3
    SHAR                        #13, R3, R4

    ;
    ; SHLL
    ;

    ; SHLL.1
    SHLL                        #13, R3

    ; SHLL.2
    SHLL                        R3, R4

    ; SHLL.3
    SHLL                        #13, R3, R4

    ;
    ; SHLR
    ;

    ; SHLR.1
    SHLR                        #13, R3

    ; SHLR.2
    SHLR                        R3, R4

    ; SHLR.3
    SHLR                        #13, R3, R4

    ;
    ; SMOVB
    ;

    ; SMOVB.1
    SMOVB

    ;
    ; SMOVF
    ;

    ; SMOVF.1
    SMOVF

    ;
    ; SMOVU
    ;

    ; SMOVU.1
    SMOVU

    ;
    ; SSTR
    ;

    ; SSTR.1
    SSTR.B
    SSTR.W
    SSTR.L

    ;
    ; STNZ
    ;

    ; STNZ.1
    STNZ                        #0FFFFFF80h, R3
    STNZ                        #00000007Fh, R3
    STNZ                        #0FFFF8000h, R3
    STNZ                        #000007FFFh, R3
    STNZ                        #0FF800000h, R3
    STNZ                        #0007FFFFFh, R3
    STNZ                        #080000000h, R3
    STNZ                        #07FFFFFFFh, R3

.IF VERSION > 1
    ; STNZ.2
    STNZ                        R3, R4
.ENDIF

    ;
    ; STZ
    ;

    ; STZ.1
    STZ                         #0FFFFFF80h, R3
    STZ                         #00000007Fh, R3
    STZ                         #0FFFF8000h, R3
    STZ                         #000007FFFh, R3
    STZ                         #0FF800000h, R3
    STZ                         #0007FFFFFh, R3
    STZ                         #080000000h, R3
    STZ                         #07FFFFFFFh, R3

.IF VERSION > 1
    ; STZ.2
    STZ                         R3, R4
.ENDIF

    ;
    ; SUB
    ;

    ; SUB.1
    SUB                         #13, R3

    ; SUB.2A
    SUB                         R1, R3
    SUB                         [R1].UB, R3
    SUB                         1[R1].UB, R3
    SUB                         256[R1].UB, R3

    ; SUB.2B
    SUB                         [R1].B, R3
    SUB                         1[R1].B, R3
    SUB                         256[R1].B, R3

    SUB                         [R1].W, R3
    SUB                         2[R1].W, R3
    SUB                         512[R1].W, R3

    SUB                         [R1].UW, R3
    SUB                         2[R1].UW, R3
    SUB                         512[R1].UW, R3

    SUB                         [R1].L, R3
    SUB                         4[R1].L, R3
    SUB                         1024[R1].L, R3

    ; SUB.3
    SUB                         R5, R3, R4

    ;
    ; SUNTIL
    ;

    ; SUNTIL.1
    SUNTIL.B
    SUNTIL.W
    SUNTIL.L

    ;
    ; SWHILE
    ;

    ; SWHILE.1
    SWHILE.B
    SWHILE.W
    SWHILE.L

    ;
    ; TST
    ;

    ; TST.1
    TST                         #0FFFFFF80h, R3
    TST                         #00000007Fh, R3
    TST                         #0FFFF8000h, R3
    TST                         #000007FFFh, R3
    TST                         #0FF800000h, R3
    TST                         #0007FFFFFh, R3
    TST                         #080000000h, R3
    TST                         #07FFFFFFFh, R3

    ; TST.2A
    TST                         R1, R3
    TST                         [R1].UB, R3
    TST                         1[R1].UB, R3
    TST                         256[R1].UB, R3

    ; TST.2B
    TST                         [R1].B, R3
    TST                         1[R1].B, R3
    TST                         256[R1].B, R3

    TST                         [R1].W, R3
    TST                         2[R1].W, R3
    TST                         512[R1].W, R3

    TST                         [R1].UW, R3
    TST                         2[R1].UW, R3
    TST                         512[R1].UW, R3

    TST                         [R1].L, R3
    TST                         4[R1].L, R3
    TST                         1024[R1].L, R3

.IF VERSION > 1
    ;
    ; UTOF
    ;

    ; UTOF.1A
    UTOF                        R3, R4
    UTOF                        [R3].UB, R4
    UTOF                        1[R3].UB, R4
    UTOF                        256[R3].UB, R4

    UTOF                        [R3].B, R4
    UTOF                        1[R3].B, R4
    UTOF                        256[R3].B, R4

    UTOF                        [R3].W, R4
    UTOF                        2[R3].W, R4
    UTOF                        512[R3].W, R4

    UTOF                        [R3].UW, R4
    UTOF                        2[R3].UW, R4
    UTOF                        512[R3].UW, R4

    UTOF                        [R3].L, R4
    UTOF                        4[R3].L, R4
    UTOF                        1024[R3].L, R4
.ENDIF

    ;
    ; WAIT
    ;

    ; WAIT.1
    WAIT

    ;
    ; XCHG
    ;

    ; XCHG.1A
    XCHG                        R3, R4

    XCHG                        R1, R3
    XCHG                        [R1].UB, R3
    XCHG                        1[R1].UB, R3
    XCHG                        256[R1].UB, R3

    ; XCHG.1B
    XCHG                        [R1].B, R3
    XCHG                        1[R1].B, R3
    XCHG                        256[R1].B, R3

    XCHG                        [R1].W, R3
    XCHG                        2[R1].W, R3
    XCHG                        512[R1].W, R3

    XCHG                        [R1].UW, R3
    XCHG                        2[R1].UW, R3
    XCHG                        512[R1].UW, R3

    XCHG                        [R1].L, R3
    XCHG                        4[R1].L, R3
    XCHG                        1024[R1].L, R3

    ;
    ; XOR 
    ;

    ; XOR.1
    XOR                         #0FFFFFF80h, R3
    XOR                         #00000007Fh, R3
    XOR                         #0FFFF8000h, R3
    XOR                         #000007FFFh, R3
    XOR                         #0FF800000h, R3
    XOR                         #0007FFFFFh, R3
    XOR                         #080000000h, R3
    XOR                         #07FFFFFFFh, R3

    ; XOR.2A
    XOR                         R1, R3
    XOR                         [R1].UB, R3
    XOR                         1[R1].UB, R3
    XOR                         256[R1].UB, R3

    ; XOR.2B
    XOR                         [R1].B, R3
    XOR                         1[R1].B, R3
    XOR                         256[R1].B, R3

    XOR                         [R1].W, R3
    XOR                         2[R1].W, R3
    XOR                         512[R1].W, R3

    XOR                         [R1].UW, R3
    XOR                         2[R1].UW, R3
    XOR                         512[R1].UW, R3

    XOR                         [R1].L, R3
    XOR                         4[R1].L, R3
    XOR                         1024[R1].L, R3

.IF VERSION == 3
    ; XOR.3
    XOR                         R3, R4, R5

    ;
    ; RSTR
    ;

    ; RSTR.1
    RSTR                        #5

    ; RSTR.2
    RSTR                        R3

    ;
    ; SAVE
    ;
    
    ; SAVE.1
    SAVE                        #5

    ; RSTR.2
    SAVE                        R3
.ENDIF    

.IF VERSION == 3
    ;
    ; DABS
    ;

    ; DABS.1
    DABS                        DR3, DR4

    ;
    ; DADD
    ;

    ; DADD.1
    DADD                        DR3, DR4, DR5

    ;
    ; DCMP
    ;

    ; DCMP.1
    DCMPUN                      DR3, DR4
    DCMPEQ                      DR3, DR4
    DCMPLT                      DR3, DR4
    DCMPLE                      DR3, DR4

    ;
    ; DDIV
    ;

    ; DDIV.1
    DDIV                        DR3, DR4, DR5

    ;
    ; DMOV
    ;

    ; DMOV.1
    DMOV.D                      R3, DRH4

    ; DMOV.2
    DMOV.L                      R3, DRH4

    ; DMOV.3
    DMOV.L                      R3, DRL4

    ; DMOV.4
    DMOV.L                      DRH3, R4

    ; DMOV.5
    DMOV.L                      DRL3, R4

    ; DMOV.6
    DMOV.D                      DR3, DR4

    ; DMOV.7
    DMOV.D                      DR3, [R4]
    DMOV.D                      DR3, 4[R4]
    DMOV.D                      DR3, 1024[R4]

    ; DMOV.8
    DMOV.D                      [R4], DR5
    DMOV.D                      4[R4], DR5
    DMOV.D                      1024[R4], DR5

    ; DMOV.9
    DMOV.D                      #03F800000h, DRH6
    DMOV.L                      #03F800000h, DRH7
    DMOV.L                      #03F800000h, DRL8

    ;
    ; DMUL
    ;

    ; DMUL.1
    DMUL                        DR3, DR4, DR5

    ;
    ; DNEG
    ;

    ; DNEG.1
    DNEG                        DR3, DR4

    ;
    ; DPOPM
    ;

    ; DPOPM.1
    DPOPM.D                     DR3-DR7

    ; DPOPM.2
    DPOPM.L                     DCR1-DCR3

    ;
    ; DPUSHM
    ;

    ; DPUSHM.1
    DPUSHM.D                    DR3-DR7

    ; DPUSHM.2
    DPUSHM.L                    DCR1-DCR3

    ;
    ; DROUND
    ;

    ; DROUND.1
    DROUND                      DR3, DR4

    ;
    ; DSQRT
    ;

    ; DSQRT.1
    DSQRT                       DR3, DR4

    ;
    ; DSUB
    ;

    ; DSUB.1
    DSUB                        DR3, DR4, DR5

    ;
    ; DTOF
    ;

    ; DTOF.1
    DTOF                        DR3, DR4

    ;
    ; DTOI
    ;

    ; DTOI.1
    DTOI                        DR3, DR4

    ;
    ; DTOU
    ;

    ; DTOU.1
    DTOU                        DR3, DR4

    ;
    ; FTOD
    ;

    ; FTOD.1
    FTOD                        R3, DR4

    ;
    ; ITOD
    ;

    ; ITOD.1
    ITOD                        R3, DR4

    ;
    ; MVFDC
    ;

    ; MVFDC.1
    MVFDC                       DCR0, R4
    MVFDC                       DCR1, R4
    MVFDC                       DCR2, R4
    MVFDC                       DCR3, R4

    ;
    ; MVFDR
    ;

    ; MVFDR.1
    MVFDR

    ;
    ; MVTDC
    ;

    ; MVTDC.1
    MVTDC                       R3, DCR0
    MVTDC                       R3, DCR1
    MVTDC                       R3, DCR2
    MVTDC                       R3, DCR3

    ;
    ; UTOD
    ;

    ; UTOD.1
    UTOD                        R3, DR4

.ENDIF

    .END
