;
;
; 汇编代码和c互相调用


REG_0_CODE      equ 0x08    ;=>00001 0 00 index=1 ti=0 rpl=00
REG_0_DATA      equ 0x10    ;=>00010 0 00 index=2 ti=0 rpl=00
REG_0_SCREEN    equ 0x18    ;=>00011 0 00 index=3 ti=0 rpl=00
LATCH equ 11931

bits 32
global _start
global my_print
extern showHello


_start:
	mov eax, REG_0_DATA
	mov ds, eax
	mov gs, eax
	; 初始化gdt
	lgdt [end_protect_model_gdt_opcode]
	;初始化内核栈
	lss esp, [ring0_stack]

	push 0xa
	lea eax, [message1]
	push eax
	call my_print
	add esp, 4

	push 0x8
	lea eax, [message2]
	push eax
	call my_print
	add esp, 4

	push 100
	lea eax, [message3]
	push eax
	call my_print
	add esp, 4


	call showHello

	jmp $










; 打印ascii字符, ring0 级运行
my_print:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push ecx
	push edx
	push ds
	push gs

	mov ebx, REG_0_DATA
	mov ds, ebx
	mov ebx, REG_0_SCREEN
	mov gs, ebx

	mov ebx, [current_coursor_positon] ;现在的输出地址位置放入ebx
	mov ecx, [ebp+0xC]                   ; ecx 存放当前字符串长度
	mov edx, [ebp+0x8]

	; 从edx 开始取ecx个字符
	.my_print_start:
	cmp ebx, 2000          ;当前位置为大于2000, 重第二行开始
	jne .my_print_reset_current_coursor_positon
	mov ebx, 0xa0
	.my_print_reset_current_coursor_positon:    ; 重置位置为第二行开始


	cmp ecx, 0             ; 长度为0，结束
	jle .my_print_end

	shl ebx, 1 ; * 2
	mov al, [edx]
	mov [gs: ebx], al
	mov byte [gs: ebx + 1], 0x02 ;color

	shr ebx, 1 ; / 2
	add ebx, 1
	add edx, 1
	sub ecx, 1
	jmp .my_print_start

	.my_print_end:
	mov [current_coursor_positon], ebx ;更新位置

	pop gs
	pop ds

	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret






align 4
current_coursor_positon:   ;四个字节大小的光标位置
	dd 0x50

align 8
protect_model_idt:
    times 2048 db 0 ;256个，每个8 bytes


message1 db "abcde12345"
message2 db "message2"
message3 db "abcde12345abcde12345abcde12345abcde12345abcde12345abcde12345abcde12345abcde12345abcde12345abcde12345"


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
