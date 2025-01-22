.data

.global probe_array
probe_array:
    .space 4096 * 256, 0x0


.text

.global vic
.align 0x1000
vic:
    pushf
    lea probe_array, %r12
    lea target, %r13
    lea safe, %r14
    mov %si, (%r13)
    mov %si, (%r14)
    xor %rax, %rax
    .rep 10
    imul %rdi, %rdi
    .endr
    mov %rdi, %rax
    shl $8, %rax
    sahf                   # save flags
    
    jc target
safe:
    nop
    nop
    mov $0x61, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b

    lahf
    popf
    ret


target:
    nop
    nop                 # 90
    mov $0x62, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    ret                 # c3