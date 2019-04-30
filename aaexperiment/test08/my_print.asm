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
extern main
global _start
_start:
    mov eax, REG_0_DATA
	mov ds, eax
	mov gs, eax
	; 初始化gdt
	lgdt [end_protect_model_gdt_opcode]
	;初始化内核栈
	lss esp, [ring0_stack]


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


    call main
    jmp $



    push 'A'
    call print_char
    add esp, 4

    push 'b'
    call print_char
    add esp, 4

    push 'C'
    call print_char
    add esp, 4

    push 'd'
    call print_char
    add esp, 4

    ; 设置中断
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

    lidt [protect_model_idt_opcode]
	; 中断初始化设置完毕
    ; 开启可屏蔽中断
	sti


    jmp $


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

    mov bl, [times_inc]
    inc bl
    mov [times_inc], bl
    cmp bl, 100
    jb .1_s_time_end

    mov byte [times_inc], 0
    push "T"
    call print_char
    add esp, 4


    .1_s_time_end:
	; 发送EOI
	mov al, 0x20
	out 0x20, al

    pop gs
    pop ds
	pop eax
    pop ebx
    iret


global set_idt_lidt
set_idt_lidt:
    push ebp
    mov ebp, esp
    push eax
    push ebx

    mov eax, [ebp+0xc]
    mov ebx, protect_model_idt_opcode
    mov word [ebx], ax
    mov eax, [ebp+0x8]
    mov dword [ebx+2], eax
    lidt [protect_model_idt_opcode]

    pop ebx
    pop eax
    pop ebp
    ret





global print_char
;---------------------- print_char ------------------------------
; 打印一个字符
;----------------------------------------------------------------
print_char:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push gs


    mov eax, SELECTOR_0_SCREEN
    mov gs, eax

    ; 获取光标位置, 先获取高8位，再获取低8位
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    in al, dx
    mov ah, al

    ; 再获取低8位
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    in al, dx


    mov bx, ax                ; bx中存放当前光标位置
    mov ecx, [ebp+0x8]        ; ecx中存放要打印字符
    ;mov ecx, [ecx]

    cmp cl, 0xd				  ;CR是0x0d,LF是0x0a
    jz .is_carriage_return
    cmp cl, 0xa
    jz .is_line_feed

    cmp cl, 0x8				  ;BS(backspace)的asc码是8
    jz .is_backspace
    jmp .put_other

    .is_backspace:
    ;退一格 设置为空格或空字符0
    dec bx
    shl bx,1
    mov byte [gs:bx], 0x20		  ;将待删除的字节补为0或空格皆可
    inc bx
    mov byte [gs:bx], 0x07
    shr bx,1
    jmp .set_cursor


    .put_other:
    shl bx, 1				  ; 光标位置是用2字节表示,将光标值乘2,表示对应显存中的偏移字节
    mov [gs:bx], cl			  ; ascii字符本身
    inc bx
    mov byte [gs:bx],0x07		  ; 字符属性
    shr bx, 1				  ; 恢复老的光标值
    inc bx				  ; 下一个光标值
    cmp bx, 2000
    jl .set_cursor			  ; 若光标值小于2000,表示未写到显存的最后,则去设置新的光标值


    .is_line_feed:				  ; 是换行符LF(\n)
    .is_carriage_return:		  ; 是回车符CR(\r) , 如果是CR(\r),只要把光标移到行首就行了。
    xor dx, dx				      ; dx是被除数的高16位,清0.
    mov ax, bx				      ; ax是被除数的低16位.   dx:ax / si = ax(商) : dx(余数)
    mov si, 80				      ; 由于是效仿linux，linux中\n便表示下一行的行首，所以本系统中，
    div si				          ; 把\n和\r都处理为linux中\n的意思，也就是下一行的行首。
    sub bx, dx				      ; 光标值减去除80的余数便是取整， 即移动到当前行行首

    .is_carriage_return_end:		  ; 回车符CR处理结束
    add bx, 80
    cmp bx, 2000
    .is_line_feed_end:			  ; 若是LF(\n),将光标移+80便可。
    jl .set_cursor

    ;屏幕行范围是0~24,滚屏的原理是将屏幕的1~24行搬运到0~23行,再将第24行用空格填充
    .roll_screen:				  ; 若超出屏幕大小，开始滚屏
    cld
    mov ecx, 960				  ; 一共有2000-80=1920个字符要搬运,共1920*2=3840字节.一次搬4字节,共3840/4=960次
    mov esi, 0xb80a0			  ; 第1行行首
    mov edi, 0xb8000			  ; 第0行行首
    rep movsd

    ;;;;;;;将最后一行填充为空白
    mov ebx, 3840			  ; 最后一行首字符的第一个字节偏移= 1920 * 2
    mov ecx, 80				  ;一行是80字符(160字节),每次清空1字符(2字节),一行需要移动80次
    .cls:
    mov word [gs:ebx], 0x0720		  ;0x0720是黑底白字的空格键
    add ebx, 2
    loop .cls
    mov bx,1920				  ;将光标值重置为1920,最后一行的首字符.

    .set_cursor:
    ;将光标设为bx值
    ; 高8位
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

    .print_char_done:
    pop gs
    pop ds
    popad
    pop ebp
    ret

global print_str
;---------------------- print_str ------------------------------
; 打印字符串
;----------------------------------------------------------------
print_str:
    push ebp
    mov ebp, esp
    push ebx
    push ecx

    xor ecx, ecx
    mov ebx, [ebp+0x8]  ; 字符串起始地址

    .next_char:
    mov cl, [ebx]
    cmp cl, 0
    jz .str_end
    push ecx
    call print_char
    add esp, 4
    inc ebx
    jmp .next_char

    .str_end:
    pop ecx
    pop ebx
    pop ebp
    ret

global print_int_oct
;---------------------- print_int_oct ------------------------------
; 打印32位整数，十进制显示
; 0 - 9 : ascii [48,57]
;----------------------------------------------------------------
print_int_oct:
    push ebp
    mov ebp, esp
    pushad
    ; 清空缓冲区的数据
    mov ebx, print_int_buffer  ; ebx->缓冲区地址
    mov ecx, 24
    .clear_int_buffer:
    xor edx, edx
    mov [ebx], dl
    dec ebx
    loop .clear_int_buffer

    mov esi, [ebp+0x8]  ; esi=》32位整数
    mov edi, print_int_buffer
    ; 32位除法

    .start_div:
    xor edx, edx
    mov eax, esi
    mov ecx, 10
    div ecx

    add edx, 48
    mov [edi], dl
    dec edi

    ; 判断商是否为0
    cmp eax, 0
    jz .div_end

    mov esi, eax
    jmp .start_div

    .div_end:
    mov esi, print_int_buffer
    sub esi, 24
    xor edx, edx
    mov ecx, 0
    .show_int_char_start:
    mov al, [esi]
    cmp dl, al
    jz .show_next_char

    push eax
    call print_char
    add esp, 4


    .show_next_char:
    inc esi

    .is_show_end:
    inc ecx
    cmp ecx, 24
    jle .show_int_char_start

    .show_int_char_end:
    popad
    pop ebp
    ret





section .my_print_data
times 24 db 0     ; 24个字节缓冲区来存放整数表示的字符
print_int_buffer:

times_inc:
db 0

align 8
protect_model_idt:
    times 2048 db 0     ;256个，每个8 bytes


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
times 512 dd 0                      ;栈的大小为512个4 bytes
ring0_stack:
	dd ring0_stack,                 ;32位偏移地址
	dw REG_0_DATA                   ;16位段选择符
