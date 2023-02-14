%macro ISR_NOERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        push byte 0
        push byte %1
        jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        push byte %1
        jmp isr_common_stub
%endmacro

%macro pushAll 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
%endmacro

%macro popAll 0
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro


ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9

ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19

ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_NOERRCODE 30
ISR_NOERRCODE 31

[EXTERN isr_handler]

isr_common_stub:
    push rax

    xor rax, rax
    mov rax, [rsp+(4*8)]
    cmp rax, 0x8
    je .skip
    swapgs
.skip:
    xor rax, rax
    mov ax, fs
    push rax

    xor rax, rax
    mov ax, es
    push rax

    xor rax, rax
    mov ax, ds
    push rax

    mov rax, 0x10
    mov fs, ax
    mov ds, ax
    mov ss, ax
    mov es, ax

    push rdi ; 5
    push rsi
    push rdx
    push rbx
    push rcx
    push rbp ;10
    push r8
    push r9
    push r10
    push r11
    push r12 ;15
    push r13
    push r14
    push r15 ; 18
    mov rdi, rsp                ; move "pointer" from rsp to rdi (first parameter)
    mov rbx, rsp
    and rsp, 0xFFFFFFFFFFFFFFF0 ; align stack
    call isr_handler
    mov rsp, rbx
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11 ;5
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rcx ;10
    pop rbx
    pop rdx
    pop rsi
    pop rdi

    pop rax ;15
    mov ds, ax

    pop rax
    mov es, ax

    pop rax
    mov fs, ax

    mov ax, [rsp+(4*8)]
    cmp rax, 0x8
    je .skip2
    swapgs
.skip2:
    pop rax ;19
    add rsp, 16
    iretq

