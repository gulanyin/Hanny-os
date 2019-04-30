;保护模式代码
;
;

bits 32 ;这是32位的指令

mov eax, 0x18
mov ds, eax

mov eax, 0x140
mov byte [eax], 'H' ;   mov byte [ds:eax] => [0000 0000 0001 1000: 0x00000000] => 第3个选择子指向显存
mov byte [eax + 1], 0x05 
add eax, 2
mov byte [eax], 'e'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'o'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], ','
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], ' '
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'w'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'o'
mov byte [eax + 1], 0x05 
add eax, 2
mov byte [eax], 'r'
mov byte [eax + 1], 0x05
add eax, 2
mov byte [eax], 'l'
mov byte [eax + 1], 0x05 
add eax, 2
mov byte [eax], 'd'
mov byte [eax + 1], 0x05 
add eax, 2
mov byte [eax], '!'
mov byte [eax + 1], 0x05



LOOP1:
    jmp LOOP1