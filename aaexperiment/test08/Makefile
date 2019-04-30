
output/all.bin: output/boots.bin output/show.bin
	cat $^ > $@
output/boots.bin: boots.asm
	nasm $^ -o $@
output/show.bin: output/temp.elf
	objcopy -O binary -S $^ $@
output/temp.elf: output/my_print.o output/main.o output/kernel.o output/interrupt.o
# output/temp.elf: output/my_print.o output/main.o
	ld -m elf_i386 -Ttext 0x0 -o $@ $^
output/my_print.o: my_print.asm
	nasm -f elf $^ -o $@
output/main.o: main.c
	gcc -m32 -c $^ -o $@
	# gcc -m32 -fno-stack-protector -c $^ -o $@
	# gcc -m32 -c $^ -o $@
output/kernel.o: kernel.asm
	nasm -f elf $^ -o $@
output/interrupt.o: interrupt.c
	gcc -m32 -c $^ -o $@
	# gcc -m32 -fno-stack-protector -c $^ -o $@
	# gcc -m32 -c $^ -o $@


clean:
	rm -rf output/*
