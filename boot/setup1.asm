; ==============================================
; setup.asm 中读取内存描述符个数和硬盘个数保存到 0x80000 和0x80004处， 0x80010处开始存放内存描述符
; setup.asm 占用3个扇区的位置
; 读取后面扇区的代码到物理内存0处，此后内核代码从物理内存0处开始执行， 内核代码从第5个扇区开始
;
; 读取硬盘b作为内核代码，FROM_HDB_SECTOR 个扇区， 读取到 0x2000:0开始处
; 在0x2000:0处， 把内核代码复制到内存0处进入保护模式，
;
; ==============================================

FROM_HDB_SECTOR equ 128

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


        ; 从硬盘b读取内核代码
        ;第1步：设置要读取的扇区数
        mov dx,0x1f2
        mov al, FROM_HDB_SECTOR          ; 内核代码占用扇区数
        out dx,al            ;读取的扇区数


          ;2 LBA
      	; high
      	mov al, 0x0
      	mov dx, 0x1f5
      	out dx, al

      	mov al, 0x0
      	mov dx, 0x1f4
      	out dx, al

      	mov al, 0x0
      	mov dx, 0x1f3
      	out dx, al

        ;3 device   111f 0000
    	mov al, 0xf0   ;从盘读取内核
    	mov dx, 0x1f6
    	out dx, al

    	; 4 发读取命令
    	mov al, 0x20
    	mov dx, 0x1f7
    	out dx, al

        .not_ready:
        ;同一端口，写时表示写入命令字，读时表示读入硬盘状态
        nop
        in al, dx
        and al, 0x88	   ;第4位为1表示硬盘控制器已准备好数据传输，第7位为1表示硬盘忙
        cmp al, 0x08
        jnz .not_ready	   ;若未准备好，继续等。


        ; 读取数据 到0x2000:0处
        mov ax, 0x2000
    	mov gs, ax
        xor bx, bx
        mov dx, 0x1f0
        mov cx, FROM_HDB_SECTOR*256  ; 每次读取两个字节 100 扇区，512字节

        .go_on_read:
        in ax, dx
        mov [gs: bx], ax
        add bx, 2
        loop .go_on_read


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
        mov si, 0      ; 跨过前面0个扇区
        xor di, di
        mov es, di

        cld               ;方向标志位 df=0
        mov cx, FROM_HDB_SECTOR*128    ; 一次移动4个字节，一个扇区移动512/4 = 128
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
