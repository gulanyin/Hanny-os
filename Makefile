GCC_FLAG = -fno-stack-protector -m32 -I include -nostdinc -fno-builtin

output/all.bin: output/boot/boots.bin output/boot/setup.bin output/boot/system.bin
	cat $^ > $@

# boot/
output/boot/boots.bin: boot/boots.asm
	nasm $^ -o $@
output/boot/setup.bin: boot/setup.asm
	nasm $^ -o $@
output/boot/system.bin: output/boot/system.elf
	objcopy -O binary -S $^ $@
output/boot/header.o: boot/header.asm
	nasm -f elf $^ -o $@
output/boot/system.elf: output/boot/header.o output/init/main.o output/kernel/print_s.o output/mm/init_memory.o \
						output/kernel/debug.o output/kernel/string.o output/kernel/bitmap.o output/kernel/interrupt_c.o \
						output/kernel/interrupt_s.o output/kernel/list_c.o  output/thread/thread_c.o output/thread/thread_s.o \
						output/kernel/sync_c.o output/device/console_c.o
	ld -m elf_i386 -Ttext 0x0  -o $@ $^


# kernel
output/kernel/print_s.o: kernel/print_s.asm
	nasm -f elf $^ -o $@
output/kernel/debug.o: kernel/debug.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/kernel/string.o: kernel/string.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/kernel/bitmap.o: kernel/bitmap.c
	gcc $(GCC_FLAG) -c $^ -o $@

output/kernel/interrupt_c.o: kernel/interrupt_c.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/kernel/interrupt_s.o: kernel/interrupt_s.asm
	nasm -f elf $^ -o $@

output/kernel/list_c.o: kernel/list_c.c
	gcc $(GCC_FLAG) -c $^ -o $@

output/kernel/sync_c.o: kernel/sync_c.c
	gcc $(GCC_FLAG) -c $^ -o $@


# init
output/init/main.o: init/main.c
	gcc $(GCC_FLAG) -c $^ -o $@


# mm
output/mm/init_memory.o: mm/init_memory.c
	gcc $(GCC_FLAG) -c $^ -o $@



# thread
output/thread/thread_c.o: thread/thread_c.c
	gcc $(GCC_FLAG) -c $^ -o $@
output/thread/thread_s.o: thread/thread_s.asm
	nasm -f elf $^ -o $@


#divice
output/device/console_c.o: device/console_c.c
	gcc $(GCC_FLAG) -c $^ -o $@


mk_dir:
	if [ ! -d "output/boot" ]; then mkdir output/boot; fi
	if [ ! -d "output/init" ];then mkdir output/init;fi
	if [ ! -d "output/kernel" ];then mkdir output/kernel;fi
	if [ ! -d "output/mm" ];then mkdir output/mm;fi
	if [ ! -d "output/thread" ];then mkdir output/thread;fi
	if [ ! -d "output/device" ];then mkdir output/device;fi

run:
	qemu-system-i386 -m size=64M -fda output/all.bin
	#qemu-system-i386 -fda output/all.bin


disk:
	nasm disk.asm -o disk.img

clean:
	rm -rf output/boot
	rm -rf output/init
	rm -rf output/kernel
	rm -rf output/mm
	rm -rf output/thread
	rm -rf output/all.bin
	if [ ! -d "output/boot" ]; then mkdir output/boot; fi
	if [ ! -d "output/init" ];then mkdir output/init;fi
	if [ ! -d "output/kernel" ];then mkdir output/kernel;fi
	if [ ! -d "output/mm" ];then mkdir output/mm;fi
	if [ ! -d "output/thread" ];then mkdir output/thread;fi
	if [ ! -d "output/device" ];then mkdir output/device;fi

test_m:
	echo "sdfdf"
	echo "aaaa"
	echo 'bbbb'
