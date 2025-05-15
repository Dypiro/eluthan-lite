bits 64

section .text
    align 4
    dd 0x1BADB002              ; magic
    dd 0x00                    ; flags
    dd - (0x1BADB002 + 0x00)   ; checksum. m+f+c should be zero

global read_port
global write_port
global load_idt

read_port:
    mov dx, di       ; move the port number from the first argument (rdi)
    in al, dx        ; read from the port
    movzx rax, al    ; zero-extend al to rax
    ret

write_port:
    mov dx, di       ; move the port number from the first argument (rdi)
    mov al, sil      ; move the value from the second argument (rsi)
    out dx, al       ; write to the port
    ret

section .bss
align 16
resb 8192           ; 8KB for stack
stack_space:
