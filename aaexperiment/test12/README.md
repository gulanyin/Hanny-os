# 写硬盘
## 当前引导扇区512字节写入磁盘
make clean </br>
make </br>
make disk</br>
qemu-system-i386 -boot order=a -fda output\all.bin -hda disk.img
