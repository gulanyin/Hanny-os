TI_GDT equ 0
RPL0 equ 0
SELECTOR_0_SCREEN equ (0x0003 << 3) + TI_GDT + RPL0  ;  => 0000 0000 0001 1000
REG_0_DATA equ (0x0002 << 3) + TI_GDT + RPL0         ; 0000 0000 0001 0000
bits 32
section .text
;---------------------- -------- ------------------------------
; 打印程序
;----------------------------------------------------------------


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
