bits 32

NUMBER_OF_INTTERRUPT_DESCRIPTOR equ 0x23    ; 已安装中断数
LATCH equ 11931 ;  1193180 / 11931 = 100.0067052216914, 一秒钟约100次中断，时钟中断处理函数中当计数器等于100打印一次

; cup 如果自动压入错误码，则nop，没有压入错误码，则用 push 0 ，后续统一 add esp, 4
%define CPU_ERROR_CODE nop
%define NO_ERROR_CODE push 0


; ------------------------------------
; 设置中断
; ------------------------------------
extern interrupt_handler_table
extern interrupt_handler_syscall_table

%macro VECTOR 2
interrupt_%1_entry:
    ; cup自动压入中断现场
    ; push ss
    ; push esp
    ; push flags
    ; push cs
    ; push eip

    ;error_code, cup自动压入或者手动压入0
    %2
    push ds
    push es
    push fs
    push gs
    pushad  ; PUSHAD指令压入32位寄存器,其入栈顺序是: EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI


    ;此处发送EOI,如果在调用 interrupt_exception_handler后发送，则切换线程后开中断没有效果
    mov al, 0x20  ; 中断结束
    out 0xa0, al
    out 0x20, al



    push %1   ; 中断号
    ; jmp interrupt_common
    ; call interrupt_exception_handler
    call [interrupt_handler_table+ %1*4]


    jmp interrupt_exit
    ; popad
    ; pop gs
    ; pop fs
    ; pop es
    ; pop ds
    ; add esp, 4   ; 平衡错误号error_code

    ; mov al, 0x20  ; 中断结束
    ; out 0xa0, al
    ; out 0x20, al
    ; iret

%endmacro

global interrupt_exit
interrupt_exit:
    add esp, 4   ; 平衡中断号
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4   ; 平衡错误号error_code
    iret


VECTOR 0x00, NO_ERROR_CODE
VECTOR 0x01, NO_ERROR_CODE
VECTOR 0x02, NO_ERROR_CODE
VECTOR 0x03, NO_ERROR_CODE
VECTOR 0x04, NO_ERROR_CODE
VECTOR 0x05, NO_ERROR_CODE
VECTOR 0x06, NO_ERROR_CODE
VECTOR 0x07, NO_ERROR_CODE
VECTOR 0x08, CPU_ERROR_CODE
VECTOR 0x09, NO_ERROR_CODE
VECTOR 0x0a, CPU_ERROR_CODE
VECTOR 0x0b, CPU_ERROR_CODE
VECTOR 0x0c, CPU_ERROR_CODE
VECTOR 0x0d, CPU_ERROR_CODE
VECTOR 0x0e, CPU_ERROR_CODE
VECTOR 0x0f, NO_ERROR_CODE

VECTOR 0x10, NO_ERROR_CODE
VECTOR 0x11, CPU_ERROR_CODE
VECTOR 0x12, NO_ERROR_CODE
VECTOR 0x13, NO_ERROR_CODE
VECTOR 0x14, NO_ERROR_CODE
VECTOR 0x15, NO_ERROR_CODE
VECTOR 0x16, NO_ERROR_CODE
VECTOR 0x17, NO_ERROR_CODE
VECTOR 0x18, NO_ERROR_CODE
VECTOR 0x19, NO_ERROR_CODE
VECTOR 0x1a, NO_ERROR_CODE
VECTOR 0x1b, NO_ERROR_CODE
VECTOR 0x1c, NO_ERROR_CODE
VECTOR 0x1d, NO_ERROR_CODE
VECTOR 0x1e, NO_ERROR_CODE
VECTOR 0x1f, NO_ERROR_CODE

VECTOR 0x20, NO_ERROR_CODE
VECTOR 0x21, NO_ERROR_CODE

; interrupt_0:
;     push 0x0
;     push 0x0
;     jmp interrupt_common

;
; interrupt_common:
;     call interrupt_exception_handler
;     add esp, 8
;     mov al, 0x20
;     out 0xa0, al
;     out 0x20, al
;     iret




; 系统调用中断处理
interrupt_0x22_entry:
    ; cup自动压入中断现场
    ; push ss
    ; push esp
    ; push flags
    ; push cs
    ; push eip

    ;error_code, cup自动压入或者手动压入0
    push 0
    push ds
    push es
    push fs
    push gs
    pushad  ; PUSHAD指令压入32位寄存器,其入栈顺序是: EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI


    push 0x22   ; 中断号
    ; jmp interrupt_common
    ; call interrupt_exception_handler
    call [interrupt_handler_syscall_table+ eax*4]
    mov [esp+ 8*4], eax  ; 返回值放入中断栈中，


    jmp interrupt_exit






interrupt_handler:
    dd interrupt_0x00_entry
    dd interrupt_0x01_entry
    dd interrupt_0x02_entry
    dd interrupt_0x03_entry
    dd interrupt_0x04_entry
    dd interrupt_0x05_entry
    dd interrupt_0x06_entry
    dd interrupt_0x07_entry

    dd interrupt_0x08_entry
    dd interrupt_0x09_entry
    dd interrupt_0x0a_entry
    dd interrupt_0x0b_entry
    dd interrupt_0x0c_entry
    dd interrupt_0x0d_entry
    dd interrupt_0x0e_entry
    dd interrupt_0x0f_entry

    dd interrupt_0x10_entry
    dd interrupt_0x11_entry
    dd interrupt_0x12_entry
    dd interrupt_0x13_entry
    dd interrupt_0x14_entry
    dd interrupt_0x15_entry
    dd interrupt_0x16_entry
    dd interrupt_0x17_entry

    dd interrupt_0x18_entry
    dd interrupt_0x19_entry
    dd interrupt_0x1a_entry
    dd interrupt_0x1b_entry
    dd interrupt_0x1c_entry
    dd interrupt_0x1d_entry
    dd interrupt_0x1e_entry
    dd interrupt_0x1f_entry

    dd interrupt_0x20_entry
    dd interrupt_0x21_entry
    dd interrupt_0x22_entry  ; 系统调用中断函数处理

; 中断描述符表
align 8
protect_model_idt:
    times NUMBER_OF_INTTERRUPT_DESCRIPTOR * 8 db 0     ;NUMBER_OF_INTTERRUPT_DESCRIPTOR个，每个8 bytes

protect_model_idt_opcode:
    dw NUMBER_OF_INTTERRUPT_DESCRIPTOR * 8 - 1
    dd protect_model_idt

io_delay:
	nop
	nop
	nop
	nop
	nop
	ret


; 设置8259A
set_pic_8259A:
    ; 设置pic 8259A
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

    ;mov al, 0xfe     ; b 11111110  开启时钟中断，屏蔽其他中断
    ; mov al, 0xfd      ; b 1111 1101 开启键盘中断
    ;mov al, 0xfc     ; b 1111 1101 开启键盘中断,时钟中断
    mov al, 0x0
    out 0x21, al
    call io_delay

    ;mov al, 0xFF
    mov al, 0x0
    out 0xA1, al
    call io_delay

    ret


set_time_frequency:
    ; 设置时钟中断频率
    mov al, 0x36
    out 0x43, al

    mov eax, LATCH
    out 0x40, al
    mov al, ah
    out 0x40, al
    ret


;设置中断向量表
set_interrupt_desc_table:
    pushad

    lea edi, [protect_model_idt]        ; edi 存放protect_model_idt 地址
    lea esi, [interrupt_handler]        ;中断函数地址

    mov ecx, 0                          ; 0x22个中断描述符, 最后一个是int 0x22 软中断，3环下访问

    ;一个中断描述符
    .repeat_set_idt_desc:
    mov edx, [esi]                     ; 中断函数地址存入edx
    mov eax, 0x00080000                ; eax高16位设置段选择符0x0008
    mov ax, dx                         ; eax存放了一个中断描述符的低32位：段选择符 + 偏移地址(低16位)
    mov dx, 0x8E00                      ; 中断门 1000 1110 0000 0000

    mov [edi], eax
    mov [edi + 4], edx
    add edi, 8                        ; 描述符表加8个字节
    add esi, 4                        ; 中断函数地址加4个字节
    inc ecx
    cmp ecx, NUMBER_OF_INTTERRUPT_DESCRIPTOR                     ; 已经设置了多少个中断描述符
    jl .repeat_set_idt_desc


    ; 系统调用，3环可以访问的中断
    lea edi, [protect_model_idt + (NUMBER_OF_INTTERRUPT_DESCRIPTOR -1 )* 8]        ; edi 存放protect_model_idt 地址
    lea esi, [interrupt_handler + (NUMBER_OF_INTTERRUPT_DESCRIPTOR -1 )* 4]        ;中断函数地址
    mov edx, [esi]                     ; 中断函数地址存入edx
    mov eax, 0x00080000                ; eax高16位设置段选择符0x0008
    mov ax, dx                         ; eax存放了一个中断描述符的低32位：段选择符 + 偏移地址(低16位)
    mov dx, 0xEE00                      ; 中断门 1110 1110 0000 0000; 3环系统调用
    mov [edi], eax
    mov [edi + 4], edx



    lidt [protect_model_idt_opcode]

    popad
    ret



global setup_interrupt
setup_interrupt:
    call set_pic_8259A
    call set_time_frequency
    call set_interrupt_desc_table
    ret
