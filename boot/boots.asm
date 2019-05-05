;
;
; boots.asm
; 引导扇区
; 把setup.asm 加载到内存0x80000处,
; setup.asm 在0x80000处通过bios中断获取到的参数保存到0x85000处
; 0x99000处的一页内存为初始化线程pcb
;



mov ax, cs
mov ds, ax
mov es, ax
mov fs, ax
mov ss, ax



;中断清屏, 0x10
mov ax, 0x0600       ; 功能号ah=0x06,
mov bx, 0xf900       ; BH = 0001 1001卷入行属性：
mov cx, 0            ; CH = 左上角行号, CL = 左上角列号
mov dx, 0x184f       ; DH = 右下角行号, DL = 右下角列号
int 0x10


; 加载后面的扇区到内存中
start_load_code_to_memery:
	mov dx, 0x0000     ; dh-磁头号， dl-驱动器号
	mov cx, 0x0002     ; ch-10位磁道号低8位，cl-位7、6 10位磁道号高2位， 位5-0 起始扇区
	                   ; 0面0道2扇区，扇区从1计数
	mov ax, 0x8000
	mov es, ax
	xor bx, bx         ; es:bx=0x8000:0000 读入缓冲区

	mov ah, 0x02       ; 功能号
	mov al, 3         ; 读入扇区数; setup.asm 占用3个扇区
	int 0x13           ; 错误cf 置1

	jnc start_load_code_to_memery_load_ok

start_load_code_to_memery_load_failed:
	; 显示错误信息
	; 直接移动到显存
	; 显存初始化地址为： B8000 - BFFFF 共 0x8000 B  32KB显存
	mov ax, 0xb800
	mov ds, ax
	mov gs, ax
	mov eax, 0  ; 从第1行开始
	mov byte [gs:eax], 'f'
	mov byte [gs:eax+1], 0x07
	mov byte [gs: eax+2], 'a'
	mov byte [gs: eax+3], 0x07
	mov byte [gs: eax+4], 'i'
	mov byte [gs: eax+5], 0x07
	mov byte [gs: eax+6], 'l'
	mov byte [gs: eax+7], 0x07
	mov byte [gs: eax+8], 'd'
	mov byte [gs: eax+9], 0x07
	jmp $


start_load_code_to_memery_load_ok:
	;加载成功
	;显示信息
	mov ax, 0xb800
	mov ds, ax
	mov gs, ax
	mov bl, 0xf3
	mov eax, 0              ; 从第1行开始
	mov byte [gs:eax], 'o'
	mov byte [gs:eax+1], bl
	mov byte [gs: eax+2], 'k'
	mov byte [gs: eax+3], bl


	jmp 0x8000:0

times 510 - ($ - $$) db 0
db 0x55
db 0xAA ;上面两行用于设置引导扇区的标识
