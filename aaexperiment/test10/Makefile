GCC_FLAG = -fno-stack-protector -m32 -I include -nostdinc -fno-builtin

output/all.bin: output/boots.bin output/second.bin
	cat $^ > $@
output/boots.bin: boots.asm
	nasm $^ -o $@
output/second.bin: output/temp.elf
	objcopy -O binary -S $^ $@
output/temp.elf: output/loader.o output/print_s.o output/interrupt_c.o output/interrupt_s.o \
	output/main.o output/debug.o output/string_c.o output/thread_c.o output/thread_s.o
	ld -m elf_i386 -Ttext 0x0 -o $@ $^

# output/main.o output/debug.o
output/loader.o: loader.asm
	nasm -f elf $^ -o $@
output/print_s.o: print_s.asm
	nasm -f elf $^ -o $@
output/interrupt_s.o: interrupt_s.asm
	nasm -f elf $^ -o $@
output/interrupt_c.o: interrupt_c.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/main.o: main.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/debug.o: debug_c.c
	gcc $(GCC_FLAG) -c $^ -o $@

output/string_c.o: string_c.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/thread_c.o: thread_c.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/thread_s.o: thread_s.asm
	nasm -f elf $^ -o $@


clean:
	rm -rf output/*
