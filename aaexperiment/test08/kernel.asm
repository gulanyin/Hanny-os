bits 32

; %define ERROR_CODE nop
; %define ZERO push 0

extern print_str
extern print_char

; section .data
; interrupt_str db "interrupt occur!", 0xa, 0x0


global interrupt_des_table

; interrupt_des_table:
;
; %macro VECTOR 2
; section .text
; interrupt%1entry:
;
;     ; %2
;     ; push interrupt_str
;     ; call print_str
;     push 'T'
;     call print_char
;     add esp, 4
;
;     mov al, 0x20
;     out 0xa0, al
;     out 0x20, al
;
;     ; add esp, 4
;     iret
;     ; jmp return_from_interrupt_handler
; ;
; ; section .data
; ;     dd interrupt%1entry
;
; %endmacro
;
;
; return_from_interrupt_handler:
;     mov al, 0x20
;     out 0xa0, al
;     out 0x20, al
;
;     add esp, 4
;     iret
;
;
; VECTOR 0x00,ZERO
; VECTOR 0x01,ZERO
; VECTOR 0x02,ZERO
; VECTOR 0x03,ZERO
; VECTOR 0x04,ZERO
; VECTOR 0x05,ZERO
; VECTOR 0x06,ZERO
; VECTOR 0x07,ZERO
; VECTOR 0x08,ERROR_CODE
; VECTOR 0x09,ZERO
; VECTOR 0x0a,ERROR_CODE
; VECTOR 0x0b,ERROR_CODE
; VECTOR 0x0c,ZERO
; VECTOR 0x0d,ERROR_CODE
; VECTOR 0x0e,ERROR_CODE
; VECTOR 0x0f,ZERO
; VECTOR 0x10,ZERO
; VECTOR 0x11,ERROR_CODE
; VECTOR 0x12,ZERO
; VECTOR 0x13,ZERO
; VECTOR 0x14,ZERO
; VECTOR 0x15,ZERO
; VECTOR 0x16,ZERO
; VECTOR 0x17,ZERO
; VECTOR 0x18,ERROR_CODE
; VECTOR 0x19,ZERO
; VECTOR 0x1a,ERROR_CODE
; VECTOR 0x1b,ERROR_CODE
; VECTOR 0x1c,ZERO
; VECTOR 0x1d,ERROR_CODE
; VECTOR 0x1e,ERROR_CODE
; VECTOR 0x1f,ZERO
; VECTOR 0x20,ZERO



interrupt0x00dddddentry:
    push 'Q'
    call print_char
    add esp, 4

    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    iret



interrupt_des_table:
    times 32 dd interrupt0x00dddddentry
    ; dd interrupt0x01entry
    ; dd interrupt0x02entry
    ; dd interrupt0x03entry
    ; dd interrupt0x04entry
    ; dd interrupt0x05entry
    ; dd interrupt0x06entry
    ; dd interrupt0x07entry
    ; dd interrupt0x08entry
    ; dd interrupt0x09entry
    ; dd interrupt0x0aentry
    ; dd interrupt0x0bentry
    ; dd interrupt0x0centry
    ; dd interrupt0x0dentry
    ; dd interrupt0x0eentry
    ; dd interrupt0x0fentry
    ; dd interrupt0x10entry
    ; dd interrupt0x11entry
    ; dd interrupt0x12entry
    ; dd interrupt0x13entry
    ; dd interrupt0x14entry
    ; dd interrupt0x15entry
    ; dd interrupt0x16entry
    ; dd interrupt0x17entry
    ; dd interrupt0x18entry
    ; dd interrupt0x19entry
    ; dd interrupt0x1aentry
    ; dd interrupt0x1bentry
    ; dd interrupt0x1centry
    ; dd interrupt0x1dentry
    ; dd interrupt0x1eentry
    ; dd interrupt0x1fentry
    ; dd interrupt0x20entry
