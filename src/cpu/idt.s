[GLOBAL gdt_flush]

gdt_flush:
    lgdt [rdi]
    lea rax, [rel .flush]
    push 0x08
    push rax
    retfq
.flush:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    retq

[GLOBAL idt_flush]

idt_flush:
    mov rax, [esp+4]
    lidt [rax]
    retq
