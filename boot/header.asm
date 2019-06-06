TI_GDT equ 0
RPL0 equ 0
SELECTOR_0_SCREEN equ (0x0003 << 3) + TI_GDT + RPL0  ;  => 0000 0000 0001 1000
REG_0_DATA equ (0x0002 << 3) + TI_GDT + RPL0         ; 0000 0000 0001 0000
LATCH equ 11931
bits 32
section .text
;---------------------- _start ------------------------------
; loader 程序开始
; 进入保护模式后，设置初始化内核栈， gdt
;----------------------------------------------------------------


;-------------------------- extern ------------------------------
extern main
extern print_char
extern init_interrupt
extern print_int_oct



;-------------------------- extern ------------------------------


global _start
_start:
    mov eax, REG_0_DATA
	mov ds, eax
	mov gs, eax   ; 设置gs,fs 后面时钟中断弹栈时防止GP
    mov fs, eax   ;
    mov es, eax
	; 初始化gdt
	lgdt [end_protect_model_gdt_opcode]
	;初始化内核栈
	lss esp, [ring0_stack]


    ; 光标设置为第2行行首
    ; 设置光标，从第二行开始打印， 光标设置为第2行行首，第80个字符
    ; 高8位
    mov bx, 80
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al
    ; 低8位
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al



    ; 初始化工作
    call main
    ; sti

    ; mov eax, 0x00100000
    ; mov CR3, eax
    ;
    ; mov eax, CR0
    ; or eax, 0x80000000
    ; mov CR0, eax

    push '\n'
    call print_char
    add esp, 4
    push 'f'
    call print_char
    add esp, 4
    push 'o'
    call print_char
    add esp, 4

    stop_run:
    ; nop
    ; nop
    ; hlt

    jmp stop_run




;新的gdt表, 增加几个描述符
align 8
global protect_model_gdt  ; 导出protect_model_gdt， 用户进程处设置tss段
protect_model_gdt:
	dw 0, 0, 0, 0

	; 第1个描述符: 代码段
	; 	G=1，b=1  limit=0xfffff*4k+4095 = 0xfffff * 0x1000 + 0x1000 -1  = 0xFFFF FFFF, 4GB
	;	base=0x00000000
	;   type = 1001 1010
	dw 0xffff
	dw 0x0000
	dw 0x9a00
	dw 0x00cf

	; 第2个描述符： 数据段,堆栈段
	; 	G=1，b=1  limit=0xfffff*4k+4095 = 0xfffff * 0x1000 + 0x1000 -1  = 0xFFFF FFFF, 4GB
	;	base=0x00000000
	;   type = 1001 1010
	dw 0xffff
	dw 0x0000
	dw 0x9200
	dw 0x00cf ;

	;第3个描述符： 显存段
	; 00C0                920b 8000 0002
	;   1100 0000 1001 0010
	; 	G=1，b=1  limit=16*4k=64k,
	;	base=0x000b8000
	;   type = 1001 0010
	dw 0x000a ;
	dw 0x8000 ;
	dw 0x920B ;
	dw 0x00c0 ;


    ;第4个描述符： tss 段, 0环
    dw 0x0067
    dw 0x0000
    dw 0x8900
    dw 0x0000


    ;第5个描述符： 3环代码 代码段
    dw 0xffff
	dw 0x0000
	dw 0xfa00
	dw 0x00cf



    ;第6个描述符： 3环数据段,堆栈段
    dw 0xffff
	dw 0x0000
	dw 0xf200
	dw 0x00cf
end_protect_model_gdt:


end_protect_model_gdt_opcode:
    dw (end_protect_model_gdt - protect_model_gdt) - 1       ;gdt的限长
                                                             ;- 1的作用和段描述符的Limit类似
                                                             ;都是表示最后一个有效的地址(<=)
    dw protect_model_gdt                                     ;gdt的起始地址
    dw 0


; 内核栈
align 8
times 512 dd 0                      ;栈的大小为512个4 bytes
ring0_stack:
    dd 0x1ff000   ; 2M内存前面空了一个页面，这个页面往前就是main线程的pcb
	; dd ring0_stack,                 ;32位偏移地址
	dw REG_0_DATA                   ;16位段选择符

























; bits 32
; section .text
;     mov eax, 1
;     mov edx, 2
;     mov ecx, 3
;     mov eax, SELECTOR_0_SCREEN
;     mov gs, eax
;
;
;     mov eax, 320
;     mov bl, 0xf3
;     mov byte [gs:eax], 'b'
;     mov byte [gs:eax+1], bl
;     add eax, 2
;     mov byte [gs:eax], 'b'
;     mov byte [gs:eax+1], bl
;     add eax, 2
;     mov byte [gs:eax], 'b'
;     mov byte [gs:eax+1], bl
;     add eax, 2
;
;     jmp $
