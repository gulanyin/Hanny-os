;
;
; 进入保护模式


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
	mov ax, 0x1000
	mov es, ax
	xor bx, bx         ; es:bx=0x1000:0000 读入缓冲区
	
	mov ah, 0x02       ; 功能号
	mov al, 10          ; 读入扇区数
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
		jmp start_load_code_to_memery_load_failed
		
		
start_load_code_to_memery_load_ok:
	;加载成功
		;显示信息
		mov ax, 0xb800
		mov ds, ax
		mov gs, ax
		mov eax, 0              ; 从第1行开始
		mov byte [gs:eax], 'o'
		mov byte [gs:eax+1], 0x07
		mov byte [gs: eax+2], 'k'
		mov byte [gs: eax+3], 0x07
		
		; 移动代码到内存地址为0的地方
		cli
		mov ax, 0x1000    ; 移动开始位置 DS:SI=0x1000:0 , 目标位置ES：DI = 0:0
		mov ds, ax
		xor si, si
		mov es, si
		xor di, di
		
		cld               ;方向标志位 df=0
		mov cx, 0x1000
		rep movsw
		
		; 设置 gdt 和 idt
		mov ax, 0x07c0    ; 加载ldt和gdt,让ds指向 ds:idt_48 = 0x07c0:idt_48
		mov ds, ax
		lidt [temp_idt_48]
		lgdt [temp_gdt_48]
		
		
		mov eax, cr0
		or al, 1
		mov cr0, eax  ;将保护模式的标记PE设置为1
		jmp dword 0x0008:0x00000000
	
	
	
	
	
; 
temp_gdt:
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
	; 00C0                920b 8000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=3*4k=12k,
	;	base=0x000b8000
	;   type = 1001 0010
	dw 0x0002 ; 
	dw 0x8000 ; 
	dw 0x920B ; 
	dw 0x00c0 ;
	

temp_idt_48:  ;32位基址，16位limit
	dw 0
	dw 0, 0
	
temp_gdt_48:
	dw 0x07ff
	dw 0x7c00+temp_gdt, 0


times 510 - ($ - $$) db 0
db 0x55
db 0xAA ;上面两行用于设置引导扇区的标识
	
	

