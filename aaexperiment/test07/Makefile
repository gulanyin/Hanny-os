
output/all.bin: output/boots.bin output/show.bin
	cat $^ > $@
output/boots.bin: boots.asm
	nasm $^ -o $@
output/show.bin: output/temp.elf
	objcopy -O binary -S $^ $@
output/temp.elf: output/my_print.o output/main.o
	ld -m elf_i386 -Ttext 0x0 -o $@ $^
output/my_print.o: my_print.asm
	nasm -f elf $^ -o $@
output/main.o: main.c
	gcc -m32 -c $^ -o $@

clean:
	rm -rf output/*
