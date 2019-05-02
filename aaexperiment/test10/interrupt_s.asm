bits 32

NUMBER_OF_INTTERRUPT_DESCRIPTOR equ 0x21
LATCH equ 11931 ;  1193180 / 11931 = 100.0067052216914, 一秒钟约100次中断，时钟中断处理函数中当计数器等于100打印一次
; ------------------------------------
; 设置中断
; ------------------------------------
extern interrupt_exception_handler

%macro VECTOR 1
interrupt_%1_entry:
    ; cup自动压入中断现场
    ; push ss
    ; push esp
    ; push flags
    ; push cs
    ; push eip
    ; 当前只处理时钟中断，error_code 为0
    push 0  ;error_code
    ; uint32_t vec_no;	 // kernel.S 宏VECTOR中push %1压入的中断号
    ; uint32_t edi;
    ; uint32_t esi;
    ; uint32_t ebp;
    ; uint32_t esp_dummy;	 // 虽然pushad把esp也压入,但esp是不断变化的,所以会被popad忽略
    ; uint32_t ebx;
    ; uint32_t edx;
    ; uint32_t ecx;
    ; uint32_t eax;
    ; uint32_t gs;
    ; uint32_t fs;
    ; uint32_t es;
    ; uint32_t ds;

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
    call interrupt_exception_handler
    add esp, 4   ; 平衡中断号

    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4   ; 平衡错误号

    ; mov al, 0x20  ; 中断结束
    ; out 0xa0, al
    ; out 0x20, al
    iret

%endmacro

VECTOR 0x00
VECTOR 0x01
VECTOR 0x02
VECTOR 0x03
VECTOR 0x04
VECTOR 0x05
VECTOR 0x06
VECTOR 0x07
VECTOR 0x08
VECTOR 0x09
VECTOR 0x0a
VECTOR 0x0b
VECTOR 0x0c
VECTOR 0x0d
VECTOR 0x0e
VECTOR 0x0f

VECTOR 0x10
VECTOR 0x11
VECTOR 0x12
VECTOR 0x13
VECTOR 0x14
VECTOR 0x15
VECTOR 0x16
VECTOR 0x17
VECTOR 0x18
VECTOR 0x19
VECTOR 0x1a
VECTOR 0x1b
VECTOR 0x1c
VECTOR 0x1d
VECTOR 0x1e
VECTOR 0x1f

VECTOR 0x20

; interrupt_0:
;     push 0x0
;     push 0x0
;     jmp interrupt_common


interrupt_common:
    call interrupt_exception_handler
    add esp, 8
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    iret

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

    mov ecx, 0                          ; 21个中断描述符

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

    lidt [protect_model_idt_opcode]

    popad
    ret



global init_interrupt
init_interrupt:
    call set_pic_8259A
    call set_time_frequency
    call set_interrupt_desc_table
    ret


global interrupt_off
interrupt_off:
    cli
    ret


global interrupt_on
interrupt_on:
    sti
    ret
