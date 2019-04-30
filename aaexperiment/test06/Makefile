
all.bin: boots.bin show.bin
	cat $^ > $@
boots.bin: boots.asm
	nasm $^ -o $@
show.bin: temp.elf
	objcopy -O binary -S $^ $@
temp.elf: header.o show.o 
	ld -m elf_i386 -Ttext 0x0 -o $@ $^
header.o: header.asm
	nasm -f elf $^ -o $@
show.o: show.c
	gcc -m32 -c $^ -o $@
	
