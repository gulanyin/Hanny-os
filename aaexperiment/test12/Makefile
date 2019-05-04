GCC_FLAG = -fno-stack-protector -m32 -I include -nostdinc -fno-builtin


output/all.bin: boots.asm
	nasm $^ -o $@

disk:
	nasm disk.asm -o disk.img

clean:
	rm -rf output/* && rm -rf disk.img
