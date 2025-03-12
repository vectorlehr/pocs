
.data

.global probe_array
probe_array:
    .space 4096 * 256, 0x0

.global mem
mem:
    .space 4096 * 256, 0x0

.text



.global vic
.align 0x1000
.rep 0x1000
nop
.endr
vic:
    
    .rep 4095
    nop
    .endr
    pushf
    lea probe_array, %r12
    lea mem, %r13
    lea safe, %r14
    mov %si, (%r13)
    mov %si, (%r14)
    jmp dump
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
    mov (%r12, %rax, 1), %r13b

    lahf
    popf
    ret


.global target
target:
    nop
    nop
    mov $0x62, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    ret                 # c3

