; ==============================================
; setup.asm 中读取内存描述符个数和硬盘个数保存到 0x80000 和0x80004处， 0x80010处开始存放内存描述符
; setup.asm 占用3个扇区的位置
; 读取后面扇区的代码到物理内存0处，此后内核代码从物理内存0处开始执行， 内核代码从第5个扇区开始
;
; int0x13 最大读取72扇区限制
;读取 ZHUMIAN_NUMBER 个柱面，软盘一个柱面有两个磁道，一个磁道18扇区，一共36扇区一个柱面
; 读取一个柱面后读到 0x1000:0 处，然后将这一个柱面移动到 0x2000:xxx处
;
; 在0x2000:xxx处， 跳过4个扇区的引导代码，把内核代码复制到内存0处进入保护模式，
;


; ==============================================

FIRST_READ_SECTOR equ 18   ; 内核代码占用扇区数
COUNT_SECTOR_OF_KENER_CODE equ 5*18  ; 内核代码占用扇区数
ZHUMIAN_NUMBER equ 3

org 0x80000

setup_read_mm:
    ; 读取内存
    mov ax, 0x8500
    mov ds, ax
    mov es, ax

    mov dword [0], 0    ; 0x85000处四字节保存内存结构体的个数
    xor ebx, ebx
    mov di, 0x10    ; es:di => 0x8500:0x10  =>85010的内存

    .continue_read:
    mov eax, 0xe820
    mov ecx, 20
    mov edx, 0x534d4150
    int 0x15

    jc .read_mm_falid   ; cf=1 则出错
    add di, 20
    inc dword [0]
    cmp ebx, 0
    jne .continue_read  ; 不为0继续读取
    jmp .read_mm_ok


    .read_mm_falid:
    mov dword [0], 0    ; 重置为0

    .read_mm_ok:
    ;此时内存信息
    ;0x00085000       00000000   <= 4个字节保存内存结构体大小
    ;0x00085010       内存描述符

    ; 保存硬盘个数到  0x00085004
    xor eax, eax
    xor ebx, ebx
    mov ds, ax
    mov bl, [0x475]  ; ax 保存了硬盘个数
    mov ax, 0x8500
    mov ds, ax
    mov dword [4], ebx

    ;
    ;读取后面扇区代码到物理内存0处，失败显示load kernel failed
    ; 加载后面的扇区到内存中

    mov dx, 0x0
    mov ax, 0x1000
    mov es, ax
    mov bx, 0
    mov ch, 0
    mov cl, 0


    setup_load_kernel_code_to_memery:


        ; .read_flappy:
        ; mov ah, 0x02
        ; mov al, 1         ; 每次读取一扇区
        ; inc cl            ; 从第一个扇区开始读
        ; int 0x13
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败
        ;
        ;
        ; .mov_select:  ; 吧0x1000:0处一扇区的移动到0x2000:0处
        ; push si
        ; push di
        ; push cx
        ;
        ; mov cx, 0x200
        ; xor si, si
        ; xor di, di   ; 移动开始位置 DS:SI=0x1000:0 , 目标位置ES：DI = 0:0




        mov ax, 0x8000
        mov gs, ax

        .read_from_fd0:   ;每读取一个柱面共36个扇区，读到0x1000:0000 处，然后将这个0x1000:0000 处移动到0x2000:0000 处
        mov ax, 0x1000
        mov es, ax
        xor bx, bx  ;es:bx=0x1000:0000
        mov dx, 0x0


        ;mov cl, 1
        mov byte ch, [gs: zhu_mian_number]  ; 柱面
        mov cl, 1
        mov ah, 0x02
        mov al, 36


        int 0x13
        jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败

        .copy_36_sectors: ;将这个0x1000:0000 处移动到0x2000:0000 处

        mov word di, [gs: dst_location]
        mov ax, 0x1000    ; 移动开始位置 DS:SI=0x1000:0 , 目标位置ES：DI = 0x2000:0
        mov ds, ax
        mov ax, 0x2000
        mov es, ax
        xor si, si

        cld               ;方向标志位 df=0
        mov cx, 36*128 ;36*36*521/2    ; 一次移动4个字节，一个扇区移动512/4 = 128
        rep movsd
        mov word [gs: dst_location], di

        mov byte al, [gs: zhu_mian_number]
        inc al
        mov [gs: zhu_mian_number], al
        cmp al, ZHUMIAN_NUMBER
        jl .read_from_fd0




        ;
        ; mov ax, 0x1000
        ; mov es, ax
        ; mov bx, 1*18*512  ;读取一个柱面，上下两个磁道，共36个扇区
        ; mov dx, 0x0
        ; mov ch, 1
        ; mov cl, 1
        ; mov ah, 0x02
        ; mov al, 18
        ; int 0x13
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败
        ;
        ; mov ax, 0x1000
        ; mov es, ax
        ; mov bx, 2*18*512
        ; mov dx, 0x0
        ; mov ch, 2
        ; mov cl, 1
        ; mov ah, 0x02
        ; mov al, 18
        ; int 0x13
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败
        ;
        ; mov ax, 0x1000
        ; mov es, ax
        ; mov bx, 3*18*512
        ; mov dx, 0x0
        ; mov ch, 3
        ; mov cl, 1
        ; mov ah, 0x02
        ; mov al, 18
        ; int 0x13
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败
        ;
        ;
        ; mov bx, 4*18*512
        ; mov dx, 0x0
        ; mov ch, 4
        ; mov cl, 1
        ; mov ah, 0x02
        ; mov al, 18
        ; int 0x13
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 读取失败


    	; mov dx, 0x0000     ; dh-磁头号， dl-驱动器号
    	; mov cx, 0x0005     ; ch-10位磁道号低8位，cl-位7、6 10位磁道号高2位， 位5-0 起始扇区
    	;                    ; 0面0道2扇区，扇区从1计数, 从第5扇区开始读取
    	; mov ax, 0x1000
    	; mov es, ax
    	; xor bx, bx         ; es:bx=0x1000:0000 读入缓冲区
        ;
    	; mov ah, 0x02       ; 功能号
    	; mov al, 70          ; 读入扇区数 ,
    	; int 0x13           ; 错误cf 置1
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 第一次读取失败

        ; 逻辑扇区号 = （面号*80+磁道号）*18+扇区号-1
        ;       那么可以使用下面的算法来计算
        ; int()：描述性运算符，取商
        ; rem()：描述性运算符，取余数
        ; 逻辑扇区号 = （面号*80+磁道号）*18+扇区号-1
        ; 面号 = int(逻辑扇区号/1440)
        ; 磁道号 = int(rem(逻辑扇区号/1440)/18)
        ; 扇区号 = rem(rem(逻辑扇区号/1440)/18)+1

        ; 逻辑扇区74号扇区开始，
        ; 面号 = int(74/1440)  = 0
        ; 磁道号 = int(rem(74/1440)/18) = 4
        ; 扇区号 = rem(rem(74/1440)/18)+1 = 3

        ; mov dx, 0x0000     ; dh-磁头号， dl-驱动器号
        ; mov cx, 0x0403     ; ch-10位磁道号低8位，cl-位7、6 10位磁道号高2位， 位5-0 起始扇区
    	;                    ; 0面0道2扇区，扇区从1计数, 从第5扇区开始读取
    	; mov bx, 70*512         ; es:bx=0x1000:0000 读入缓冲区
        ;
    	; mov ah, 0x02       ; 功能号
    	; mov al, 70          ; 读入扇区数 ,
    	; int 0x13           ; 错误cf 置1
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 第一次读取失败


        ; 第二次读取 从第二个柱面开始读取, 读取两个柱面，36个扇区
        ; mov ch, 2
        ; mov cl, 0
        ; mov bx, 13*512
        ; mov ah, 0x02       ; 功能号
    	; mov al, 36          ; 读入扇区数 , 先读乳18个扇区数 一个柱面18个扇区
    	; int 0x13           ; 错误cf 置1
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 第一次读取失败
        ;
        ;
        ; ; 第3次读取 从第4个柱面开始读取, 读取两个柱面，36个扇区
        ; mov ch, 4
        ; mov cl, 0
        ; mov bx, (13+36)*512
        ; mov ah, 0x02       ; 功能号
    	; mov al, 36          ; 读入扇区数 , 先读乳18个扇区数 一个柱面18个扇区
    	; int 0x13           ; 错误cf 置1
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 第一次读取失败
        ;
        ;
        ; ; 第3次读取 从第6个柱面开始读取, 读取两个柱面，36个扇区
        ; mov ch, 6
        ; mov cl, 0
        ; mov bx, (13+36*2)*512
        ; mov ah, 0x02       ; 功能号
    	; mov al, 36          ; 读入扇区数 , 先读乳18个扇区数 一个柱面18个扇区
    	; int 0x13           ; 错误cf 置1
        ; jc setup_load_kernel_code_to_memery_load_failed  ; 第一次读取失败

    	;jnc setup_load_kernel_code_to_memery_load_ok
        jmp setup_load_kernel_code_to_memery_load_ok

    setup_load_kernel_code_to_memery_load_failed:
    	; 显示错误信息 load kernel failed
    	mov ax, 0xb800
    	mov gs, ax
    	mov eax, 0xa0  ; 从第1行开始
        mov bl, 0xf3
    	mov byte [gs:eax], 'l'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'o'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'a'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'd'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], ' '
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'k'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'r'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'r'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], ' '
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'f'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'a'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'i'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'l'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
    	mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'd'
    	mov byte [gs:eax+1], bl
        add eax, 2
    	jmp $


    setup_load_kernel_code_to_memery_load_ok:
    	;加载成功
        ; 移动代码到内存地址为0的地方

        cli
        mov ax, 0x2000    ; 移动开始位置 DS:SI=0x2000:2048 , 目标位置ES：DI = 0:0
        mov ds, ax
        mov si, 2048      ; 跨过前面几个扇区
        xor di, di
        mov es, di

        cld               ;方向标志位 df=0
        mov cx, ZHUMIAN_NUMBER*36*128    ; 一次移动4个字节，一个扇区移动512/4 = 128
        rep movsd


    	;显示信息
        mov ax, 0xb800
    	mov gs, ax
    	mov eax, 0xa0  ; 从第1行开始
        mov bl, 0xf3
        mov byte [gs:eax], 'k'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'r'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'e'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'n'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'r'
        mov byte [gs:eax+1], bl
        add eax, 2
        add eax, 2
        mov byte [gs:eax], ' '
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'o'
        mov byte [gs:eax+1], bl
        add eax, 2
        mov byte [gs:eax], 'k'
        mov byte [gs:eax+1], bl
        add eax, 2



        add ebx, 1
        add ebx, 1
        mov ax, 0x8000
        mov ds, ax
        ;初始化gdt
        lidt [temp_idt_48]
    	lgdt [temp_gdt_48]
        mov eax, cr0
    	or al, 1
    	mov cr0, eax  ;将保护模式的标记PE设置为1
    	jmp dword 0x0008:0x00000000
        jmp $


dst_location:
dw 0
zhu_mian_number:
db 0


;新的gdt表, 增加几个描述符
align 8
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
temp_gdt_end:

temp_idt_48:  ;32位基址，16位limit
	dw 0
	dw 0, 0

temp_gdt_48:
	dw (temp_gdt_end - temp_gdt) - 1
	dd temp_gdt


times 512*3 - ($ - $$) db 0  ; 占用3个扇区
