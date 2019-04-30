;
;中断实验
;


REG_0_CODE      equ 0x08    ;=>00001 0 00 index=1 ti=0 rpl=00
REG_0_DATA      equ 0x10    ;=>00010 0 00 index=2 ti=0 rpl=00
REG_0_SCREEN    equ 0x18    ;=>00011 0 00 index=3 ti=0 rpl=00
LATCH equ 11931

bits 32

	cli                             ; 关中断
	
	
	; 重置ds数据段寄存器
	mov eax, REG_0_DATA
	mov ds, eax
	mov gs, eax  ; gs 在时钟中断里要pop,此时会进行段检查
	

	lgdt [end_protect_model_gdt_opcode]
	lidt [protect_model_idt_opcode]
	
	;初始化内核栈
	lss esp, [ring0_stack]

	
	
	; 初始化 8259A
	; 只允许时钟中断
	mov al, 0x11
	out 0x20, al 
	call io_delay    ; 主片 ICW1
	
	out 0xa0, al
	call io_delay    ; 从片 ICW1
	
	mov al, 0x20     
	out 0x21, al     ; 主片 ICW2
	call io_delay    ; 主片中断向量 0x20开始
	
	mov al, 0x28
	out 0xa1, al     ; 从片 ICW2
	call io_delay    ; 主片中断向量 0x28开始
	
	mov al, 0x04
	out 0x21, al
	call io_delay    ; 主片ICW3
	
	mov al, 0x02
	out 0xa1, al
	call io_delay    ; 从片ICW3
	
	mov al, 0x01
	out 0x21, al
	call io_delay    ; 主片ICW4
	
	out 0xa1, al
	call io_delay    ; 从片ICW4
	
	mov al, 0xfe     ; b 11111110  开启时钟中断，屏蔽其他中断
	out 0x21, al
	call io_delay
	
	mov al, 0xFF
	out 0xA1, al
	call io_delay
	
	
	; 设置时钟中断频率
	mov al, 0x36
	out 0x43, al

	mov eax, LATCH
	out 0x40, al
	mov al, ah
	out 0x40, al
	
	
	
	
	
	; 此时时钟中断的向量号为 0x20+1 = 0x21
	; 初始化idt表
	; 设置初始的默认中断描述符
	lea edx, [int_ignore]               ; 将int_ignore的地址写入edx
	mov eax, 0x00080000                 ; eax高16位设置段选择符0x0008
	mov ax, dx                          ; eax存放了一个中断描述符的低32位：段选择符 + 偏移地址(低16位)
          
	mov dx, 0x8E00                      ; 中断门 1000 1110 0000 0000
						                ; P = 1, DPL = 00               
                                        ; 目前为止，edx为中断门的高32位
	
	lea edi, [protect_model_idt]        ; 将idt的地址写入edi
	mov ecx, 256                        ; 一共256个中断描述符
	
	; 循环初始化所有中断门为int_ignore
	rp_idt:
		mov [edi], eax
		mov [edi + 4], edx
		add edi, 8
		dec ecx
		jne rp_idt
		
	; 设置时钟中断
	; 重新初始化时钟中断0x08
	lea edx, [int_timer]
	mov eax, 0x00080000 ;段选择符0x08
	mov ax, dx
	mov dx, 0x8E00 ;P DPL ...
	mov ecx, 0x20 ;时钟中断号
	lea edi, [protect_model_idt + ecx * 8] ;中断的偏移地址放到edi
	mov [edi], eax
	mov [edi + 4], edx
	
	; 中断初始化设置完毕
    ; 开启可屏蔽中断
	sti
	
	
	print_info:
	;jmp print_info
		mov ebx, REG_0_DATA
		mov ds, ebx

		mov ebx, REG_0_SCREEN
		mov gs, ebx
		mov ebx, [current_coursor_positon] ;现在的输出地址

		shl ebx, 1 ; * 2
		mov byte [gs: ebx], 'A'
		mov byte [gs: ebx + 1], 0x02 ;color

		shr ebx, 1 ; / 2
		add ebx, 1

		cmp ebx, 2000 ; 80 * 25
		jne .print_info_mark_update_loc

		mov ebx, 0xa0
		.print_info_mark_update_loc:
		mov [current_coursor_positon], ebx ;更新位置
		
		
		; 显示一个A后等待时钟中断唤醒
		;mov ebx, 0xffff0000
		;delay_loop:
			hlt
			;sub ebx, 1
			;cmp ebx, 0
			;jge delay_loop
		
	
	jmp print_info
	

io_delay:
	nop
	nop
	nop
	nop
	nop
	ret




align 4
int_ignore: ; 默认的硬件中断处理函数
    iret


align 4
int_timer:  ; 时钟中断处理函数

	push ebx
	push eax
    push ds
    push gs
	
	

    mov ebx, REG_0_DATA
	mov ds, ebx

    mov ebx, REG_0_SCREEN
    mov gs, ebx
    mov ebx, [current_coursor_positon] ;现在的输出地址

    shl ebx, 1 ; * 2
    mov byte [gs: ebx], 'T'
    mov byte [gs: ebx + 1], 0x02 ;color

    shr ebx, 1 ; / 2
    add ebx, 1

    cmp ebx, 2000 ; 80 * 25
    jne .mark_update_loc

    mov ebx, 0
    .mark_update_loc:
    mov [current_coursor_positon], ebx ;更新位置


	; 发送EOI
	mov al, 0x20
	out 0x20, al

    pop gs
    pop ds
	pop eax
    pop ebx
    iret


align 4
current_coursor_positon:   ;四个字节大小的光标位置
	dd 0xa0

align 8
protect_model_idt:
    times 2048 db 0 ;256个，每个8 bytes



;新的gdt表, 增加几个描述符
align 8
protect_model_gdt:
	dw 0, 0, 0, 0
	
	; 第1个描述符: 代码段
	; 00c0                9a00 0000 07ff
	;   1100 0000 1001 1010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff   
	dw 0x0000
	dw 0x9a00
	dw 0x00c0
	
	; 第2个描述符： 数据段,堆栈段
	; 00c0                9200 0000 07ff
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff
	dw 0x0000
	dw 0x9200
	dw 0x00c0 ; 
	
	;第3个描述符： 显存段
	; 00C0                920b 8000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=3*4k=12k,
	;	base=0x000b8000
	;   type = 1001 0010
	dw 0x0002 ; 
	dw 0x8000 ; 
	dw 0x920B ; 
	dw 0x00c0 ;
		
	
end_protect_model_gdt:
end_protect_model_gdt_opcode:
    dw (end_protect_model_gdt - protect_model_gdt) - 1       ;gdt的限长
                                                             ;- 1的作用和段描述符的Limit类似
                                                             ;都是表示最后一个有效的地址(<=)
    dw protect_model_gdt                                     ;gdt的起始地址
    dw 0

protect_model_idt_opcode:
    dw 256 * 8 - 1
    dd protect_model_idt


; 内核栈
align 8
times 256 dd 0                      ;栈的大小为256个4 bytes
ring0_stack:
	dd ring0_stack,                 ;32位偏移地址
	dw REG_0_DATA                   ;16位段选择符
