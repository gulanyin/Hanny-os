;
;
; 进入保护模式


mov ax, cs
mov ds, ax
mov es, ax
mov fs, ax
mov ss, ax

	mov ax, 0x7c0
	mov ds, ax
	mov esi, 0
	mov eax, [esi]
; 写硬盘操作
 	;1 设置写扇区数
	mov dx, 0x1f2
	mov al, 0x1
	out dx, al

	;2 LBA
	; high
	mov al, 0x0
	mov dx, 0x1f5
	out dx, al

	mov al, 0x0
	mov dx, 0x1f4
	out dx, al

	mov al, 0x0
	mov dx, 0x1f3
	out dx, al

	;3 device   1110 0000
	mov al, 0xe0
	mov dx, 0x1f6
	out dx, al

	; 4 发写命令
	mov al, 0x30
	mov dx, 0x1f7
	out dx, al





	xor cx, 0

	write:
	mov dx, 0x1f0
	outsw

	inc cx
	cmp cx, 256
	jae wirte_end


	waits:
	mov dx, 0x1f7
	in al, dx
	and al, 0x88
	cmp al, 0x08
	jnz waits
	jmp write


	wirte_end:
	add ax, 1



	; 显示错误信息
	; 直接移动到显存
	; 显存初始化地址为： B8000 - BFFFF 共 0x8000 B  32KB显存
	mov ax, 0xb800
	mov ds, ax
	mov gs, ax
	mov eax, 0  ; 从第1行开始
	mov bl, 0xf3
	mov byte [gs:eax], 'w'
	mov byte [gs:eax+1], bl
	mov byte [gs: eax+2], 'r'
	mov byte [gs: eax+3], bl
	mov byte [gs: eax+4], 'i'
	mov byte [gs: eax+5], bl
	mov byte [gs: eax+6], 't'
	mov byte [gs: eax+7], bl
	mov byte [gs: eax+8], 'e'
	mov byte [gs: eax+9], bl
	mov byte [gs: eax+10], 'o'
	mov byte [gs: eax+11], bl
	mov byte [gs: eax+12], 'k'
	mov byte [gs: eax+13], bl
	jmp $


my_data:
	db 0x66, 0xBB

times 510 - ($ - $$) db 0
db 0x55
db 0xAA ;上面两行用于设置引导扇区的标识
