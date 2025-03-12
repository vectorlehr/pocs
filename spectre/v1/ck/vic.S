
.text

.global vic
.align 0x1000
vic:
    
    .rep 4095
    nop
    .endr
    pushf
    lea safe, %r14
    mov %si, (%r14)
    # mfence

dump:
    nop
    nop
    .rep 10
    imul $1, %rdi
    .endr
    mov %rdi, %rax
    shl $8, %rax
    sahf                   # save flags
    
    jc target

.global safe
safe:
    nop
    nop
    mov $0x61, %rax
    shl $12, %rax
    mov (%rdx, %rax, 1), %r13b

    lahf
    popf
    ret


.global target
target:
    nop
    nop
    mov $0x40, %rax
    shl $12, %rax
    mov (%rdx, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    ret                 # c3



