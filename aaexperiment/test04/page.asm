;
; 分页机制
;

REG_0_CODE      equ 0x08    ;=>00001 0 00 index=1 ti=0 rpl=00
REG_0_DATA      equ 0x10    ;=>00010 0 00 index=2 ti=0 rpl=00
REG_0_SCREEN    equ 0x18    ;=>00011 0 00 index=3 ti=0 rpl=00


bits 32



;只给页目录表的第0项赋值
;赋值为物理地址1M+4K的高20位，即页表项

;先设置数据段，重置gdtr 让后面的mov eax, REG_0_SCREEN时
;可以根据gtdr寄存器里的线性地址根据分页来找到物理地址存在的描述符，否则实模式下的设置的gdtr指向的内存给ds赋值时会引发检查错误

mov eax, REG_0_DATA 
mov ds, eax
lgdt [end_protect_model_gdt_opcode]


;设置页目录表基址， 页目录表物理地址1M的地方，页目录的第一项指向第一个页表
;第一个页表在1M+4*1024的位置，紧挨着页目录表
mov eax, 0x00100000 
mov CR3, eax

;0x00101001的高20位表示页桢地址0x00101，页目录的第一项指向第一个页表
mov dword [0x00100000], 0x00101007

; 开启分页后，eip 指向的逻辑地址指令也会遵循分页机制
mov dword [0x00100000 + 4*1024+0*4], 0x00000007      ; 指向物理内存0起始的第一页，就是当前代码
mov dword [0x00100000 + 4*1024+1*4], 0x000b8007      ; 指向物理内存0xb8000起始的一页，就是显存

;开启分页模式
;即将CR0寄存器的第31位置1
mov eax, CR0
or eax, 0x80000000 ;最高位置1
mov CR0, eax


; 此处给ds段寄存器复制的时候，gtdr寄存器里的线性地址根据分页来找到物理地址，因此上面重置了gtdr寄存器让他们在同一页中
mov eax, REG_0_SCREEN
mov ds, eax

mov eax, 0xa0
mov byte [eax], 'H'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'e'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'o'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], ','
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], ' '
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'w'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'o'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'r'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], 'd'
mov byte [eax + 1], 0x02 ;color
add eax, 2
mov byte [eax], '!'
mov byte [eax + 1], 0x02 ;color

LOOP1:
    jmp LOOP1
	
	
;新的gdt表, 增加几个描述符
align 8
protect_model_gdt:
	dw 0, 0, 0, 0
	
	; 第一个段: 代码段
	; 00c0                9a00 0000 07ff
	;   1100 0000 1001 1010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff   
	dw 0x0000
	dw 0x9a00
	dw 0x00c0
	
	; 第二个段： 数据段,堆栈段
	; 00c0                9200 0000 07ff
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff
	dw 0x0000
	dw 0x9200
	dw 0x00c0 ; 
	
	;第三个段： 显存段
	; 00C0                9200 1000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=3*4k=12k,
	;	base=0x00001000           =>起始地址b: 0000 0000 00 00 0001 0000 0000 0000 指向第一个页表的第二项，该项指向物理内存显存位置
	;   type = 1001 0010
	dw 0x0002 ; 
	dw 0x1000 ; 
	dw 0x9200 ; 
	dw 0x00c0 ;
end_protect_model_gdt:
end_protect_model_gdt_opcode:
    dw (end_protect_model_gdt - protect_model_gdt) - 1       ;gdt的限长
                                                             ;- 1的作用和段描述符的Limit类似
                                                             ;都是表示最后一个有效的地址(<=)
    dw protect_model_gdt                                     ;gdt的起始地址
    dw 0

