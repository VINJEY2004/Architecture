# 07_ISA/code/01_disasm/RX

Round trip testing:
Round trip testing is done on disassembler testing as noted below

    \leveltwo
    (a) Ref. ASM $\Rightarrow$ CC-RX $\Rightarrow$ Object File $\Rightarrow$ BIN File

    (b) BIN File $\Rightarrow$ Our Disassembler $\Rightarrow$ DUT ASM

Steps are as given below

    (a) Reference assembly file is converted to object file using CC-RX

    (b) .text section containing code is extract to a binary file using objcopy

    (c) Binary file is converted to Device Under Test (DUT) assembly using our disassembler

Steps
I. Get bin file for input

A. Without Labels

To test with v1
1. Change VERSION value to 1
2. ccrx -fpu -nologo -dbl_size=8 -isa=rxv1 input.s
3. rx-elf-objcopy --dump-section P=input.bin input.obj

To test with v2
1. Change VERSION value to 2
2. ccrx -fpu -nologo -dbl_size=8 -isa=rxv2 input.s
3. rx-elf-objcopy --dump-section P=input.bin input.obj

To test with v3
1. Change VERSION value to 3
2. ccrx -fpu -dpfpu -nologo -dbl_size=8 -isa=rxv3 input.s
3. rx-elf-objcopy --dump-section P=input.bin input.obj

B. With Labels

1. ccrx -fpu -dpfpu -nologo -dbl_size=8 -isa=rxv3 input_label.s
2. rx-elf-gcc -nostdlib input_label.obj -o input_label.out
3. rx-elf-objcopy --dump-section .text=input_label.bin input_label.out

II. Compile and run disassembler

