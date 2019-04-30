;
;调用门 特权级转换
;
;

REG_0_CODE      equ 0x08    ;=>00001 0 00 index=1 ti=0 rpl=00
REG_0_DATA      equ 0x10    ;=>00010 0 00 index=2 ti=0 rpl=00
REG_0_SCREEN    equ 0x18    ;=>00011 0 00 index=3 ti=0 rpl=00
REG_0_CALL_GATE equ 0x23    ;=>00100 0 11 index=4 ti=0 rpl=11

REG_3_CODE      equ 0x2b    ;=>00101 0 11 index=5 ti=0 rpl=11
REG_3_DATA      equ 0x33    ;=>00110 0 11 index=6 ti=0 rpl=11
REG_3_SCREEN    equ 0x3b    ;=>00111 0 11 index=7 ti=0 rpl=11

REG_TSS         equ 0x40    ;=>01000 0 00 index=8 ti=0 rpl=00 





bits 32 ;这是32位的指令

	mov eax, REG_0_DATA
	mov ds, eax
	
	; 设置好调用门描述符
	lea edi, [protect_model_gdt] ;将 protect_model_gdt 的地址写入edx， [edx + 8*4] 调用门描述符
	mov eax, 0x00080000          ; 调用门中选择子0x08
	lea edx, [ring_0_show_str]
	mov ax, dx
	mov dx,  0xec00  ;1110 1100 0000 0000
	mov [edi + 8*4], eax
	mov [edi + 8*4+4], edx
	
	
	

	lgdt [end_protect_model_gdt_opcode]
	;初始化内核栈
	lss esp, [ring0_stack]
	mov eax, REG_TSS
	
	; 设置tss
	ltr ax


	mov eax, REG_0_SCREEN
	mov ds, eax
	mov eax, (3-1)*80*2          ;第三行开始显示
	mov byte [eax], '0'   
	mov byte [eax + 1], 0x05 


	; 构造返回环境
	push REG_3_DATA       ;ring_3_ss
	push ring3_stack      ;ring_3_esp
	push REG_3_CODE       ;ring_3_cs
	push ring_3_show_str  ;ring_3_eip
	retf  ;返回ring 3



ring_0_show_str:
	mov eax, REG_0_SCREEN
	mov ds, eax
	mov eax, (5-1)*80*2          ;第5行开始显示
	mov byte [eax], '0'   
	mov byte [eax + 1], 0x05 
	
	retf                         ;返回ring 3




ring_3_show_str:
	mov eax, REG_3_SCREEN
	mov ds, eax
	mov eax, (4-1)*80*2          ;第4行开始显示
	mov byte [eax], '3'   
	mov byte [eax + 1], 0x05 
	
	; 调用门 到 ring0
	
	call dword REG_0_CALL_GATE:0x0
	
	mov eax, REG_3_SCREEN
	mov ds, eax
	mov eax, (6-1)*80*2
	mov byte [eax], '3'          ;第6行开始显示
	mov byte [eax + 1], 0x05 
	
	jmp $








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
	
	;第4个描述符： 调用门
	; 00C0                920b 8000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=3*4k=12k,
	;	base=0x000b8000
	;   type = 1001 0010
	dw 0x0002 ; 
	dw 0x8000 ; 
	dw 0x920B ; 
	dw 0x00c0 ;
	
	
	
	; 第5个描述符: ring3 代码段
	; 00c0                9a00 0000 07ff
	;   1100 0000 1111 1010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff   
	dw 0x0000
	dw 0xfa00
	dw 0x00c0
	
	; 第6个描述符： ring3 数据段,堆栈段
	; 00c0                9200 0000 07ff
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=0x007ff*4k+4095 = 8M-1,
	;	base=0x00000000
	;   type = 1001 1010
	dw 0x07ff
	dw 0x0000
	dw 0xf200
	dw 0x00c0 ; 
	
	;第7个描述符： ring3 显存段
	; 00C0                920b 8000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=3*4k=12k,
	;	base=0x000b8000
	;   type = 1001 0010
	dw 0x0002 ; 
	dw 0x8000 ; 
	dw 0xf20b ; 
	dw 0x00c0 ;
	
	
	
	;第8个描述符： tss段
	; 0000                e900 tss_seg 0x68
	;   0000 0000 1001 0010
	; 	G=0，b=0  limit=0x68,
	;	base=0x000tss_seg
	;   type = 1110 1001=》32位tss
	dw 0x68 ; 
	dw tss_seg ; 
	dw 0xe900 ; 
	dw 0x0000 ;
	
	
	
	
end_protect_model_gdt:
end_protect_model_gdt_opcode:
    dw (end_protect_model_gdt - protect_model_gdt) - 1       ;gdt的限长
                                                             ;- 1的作用和段描述符的Limit类似
                                                             ;都是表示最后一个有效的地址(<=)
    dw protect_model_gdt                                     ;gdt的起始地址
    dw 0




; 内核栈
align 8
times 256 dd 0                      ;栈的大小为256个4 bytes
ring0_stack:
	dd ring0_stack,                 ;32位偏移地址
	dw REG_0_DATA                   ;16位段选择符

; 用户栈
times 256 dd 0                      ;栈的大小为256个4 bytes
ring3_stack:











; tss段
tss_seg:
	dd 0           ;back link
	dd ring0_stack, REG_0_DATA
	dd 0, 0, 0, 0, 0         ; esp1, ss1, esp2, ss2, cr3
	dd 0, 0, 0, 0, 0         ; eip, eflags, eax, ecx, edx
	dd 0, 0, 0, 0, 0         ; ebx, esp, ebp, esi, edi
	dd 0, 0, 0, 0, 0, 0      ; es, cs, ss, ds, fs, gs
	dd 0                     ; ldt
	dd 0                     ; trace bit map
	
	
