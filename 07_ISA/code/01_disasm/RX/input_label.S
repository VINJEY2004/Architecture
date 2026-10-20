;
; Copyright VINJEY Software Systems Pvt. Ltd, All Rights Reserved.
;
; All rights reserved. Any attempt or participation in deciphering, decoding,
; reverse engineering or in any way altering the source code is strictly
; prohibited, unless the prior written consent of VINJEY is obtained.
;
; Filename         :  input_label.S
; Description      :  Assembly Input Test code for disassembler validation
; Version Number   :  1.0
;
; 1. ccrx -fpu -dpfpu -nologo -dbl_size=8 -isa=rxv3 input_label.s
; 2. rx-elf-gcc -nostdlib input_label.obj -o input_label.out
; 3. rx-elf-objcopy --dump-section .text=input_label.bin input_label.out
;

    .section P, CODE
    .GLB _start

    ;
    ; BCnd
    ;
_start:
    ; BCnd.1
    BEQ.S                    label1
    NOP
    NOP
label1:

    BEQ.S                    label2
    NOP
    NOP
    NOP
label2:

    BEQ.S                    label3
    NOP
    NOP
    NOP
    NOP
label3:

    BEQ.S                    label4
    NOP
    NOP
    NOP
    NOP
    NOP
label4:

    BEQ.S                    label5
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
label5:

    BEQ.S                    label6
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
label6:

    BEQ.S                    label7
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
label7:

    BEQ.S                    label8
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
label8:

    BNE.S                    nlabel1
    NOP
    NOP
nlabel1:

    BNE.S                    nlabel2
    NOP
    NOP
    NOP
nlabel2:

    BNE.S                    nlabel3
    NOP
    NOP
    NOP
    NOP
nlabel3:

    BNE.S                    nlabel4
    NOP
    NOP
    NOP
    NOP
    NOP
nlabel4:

    BNE.S                    nlabel5
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
nlabel5:

    BNE.S                    nlabel6
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
nlabel6:

    BNE.S                    nlabel7
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
nlabel7:

blabel:
    BNE.S                    nlabel8
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
nlabel8:

    ; BCnd.2
    BEQ.B                    blabel
    BZ.B                     blabel
    BNE.B                    blabel
    BNZ.B                    blabel
    BGEU.B                   blabel
    BC.B                     blabel
    BLTU.B                   blabel
    BNC.B                    blabel
    BGTU.B                   blabel
    BLEU.B                   blabel
    BPZ.B                    blabel
    BN.B                     blabel
    BGE.B                    blabel
    BLT.B                    blabel
    BGT.B                    blabel
    BLE.B                    blabel
    BO.B                     blabel
    BNO.B                    blabel

    BEQ.B                    flabel
    BZ.B                     flabel
    BNE.B                    flabel
    BNZ.B                    flabel
    BGEU.B                   flabel
    BC.B                     flabel
    BLTU.B                   flabel
    BNC.B                    flabel
    BGTU.B                   flabel
    BLEU.B                   flabel
    BPZ.B                    flabel
    BN.B                     flabel
    BGE.B                    flabel
    BLT.B                    flabel
    BGT.B                    flabel
    BLE.B                    flabel
    BO.B                     flabel
    BNO.B                    flabel

flabel:

    ; BCnd.3
    BEQ.W                    blabel
    BZ.W                     blabel
    BNE.W                    blabel
    BNZ.W                    blabel

flabel2:


    ;
    ; BRA
    ;

    ; BRA.1
    BRA.S                    ulabel1
    NOP
    NOP
ulabel1:

    BRA.S                    ulabel2
    NOP
    NOP
    NOP
ulabel2:

    BRA.S                    ulabel3
    NOP
    NOP
    NOP
    NOP
ulabel3:

    BRA.S                    ulabel4
    NOP
    NOP
    NOP
    NOP
    NOP
ulabel4:

    BRA.S                    ulabel5
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
ulabel5:

    BRA.S                    ulabel6
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
ulabel6:

    BRA.S                    ulabel7
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
ulabel7:

ublabel:
    BRA.S                    ulabel8
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
ulabel8:

    ; BRA.2
    BRA.B                    ublabel
    BRA.B                    uflabel

    ; BRA.3
    BRA.W                    ublabel
    BRA.W                    uflabel

    ; BRA.4
    BRA.A                    ublabel
    BRA.A                    uflabel

    ; BRA.5
    BRA.L                    R3

uflabel:

    ;
    ; BSR
    ;

sblabel:
    NOP

    ; BSR.1
    BSR.W                    sblabel
    BSR.W                    sflabel

    ; BSR.2
    BSR.A                    sblabel
    BSR.A                    sflabel

    ; BSR.3
    BSR.L                    R3

sflabel:

    .END
