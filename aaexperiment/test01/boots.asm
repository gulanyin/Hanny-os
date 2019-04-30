;
; 简单的引导扇区
;

org 0x7c00
;
;
;  利用bois 中断来显示文字
mov ax, cs
mov ds, ax
mov es, ax
mov fs, ax
mov ss, ax

;中断清屏
mov ax, 0x0600
mov bx, 0xf900  ; BH = 0001 1001卷入行属性：
mov cx, 0       ; 左上角: (0, 0)
mov dx, 0x184f  ; VGA文本模式中,一行只能容纳80个字符,共25行，右下角0x18=24,0x4f=79
int 0x10

; bois 中断显示字符串
mov ax, message
mov bp, ax
mov cx, 10
mov dx, 0x01
mov bx, 0x02
mov ax, 0x1301
int 0x10

; 直接移动到显存
; 显存初始化地址为： B8000 - BFFFF 共 0x8000 B  32KB显存
mov ax, 0xb800
mov ds, ax
mov gs, ax
mov eax, 0xa0  ; 从第二行开始
mov byte [gs:0xa0], 'L'
mov byte [gs:0xa0+1], 0x07

mov byte [gs: eax+2], 'B'
mov byte [gs: eax+3], 0x0F
mov byte [gs: eax+4], 'C'
mov byte [gs: eax+5], 0xF4; F4 = 1111 0100  KRGBIRGB 背景色K1 GRB: 111 前景色：I0 RGB:000

jmp $

message db "abcde12345"
times 510-($-$$) db 0
db 0x55
db 0xaa