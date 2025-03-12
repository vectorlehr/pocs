.data

.global probe_array
probe_array:
    .space 4096 * 256, 0x0

.global mem
mem:
    .space 4096 * 256, 0x0

.text


.global stub
stub:


.global vic
.align 0x1000
vic:
    push %rbp
    mov %rsp, %rbp
    nop
    nop
    pushf
    lea probe_array, %r12
    lea mem, %r13
    lea safe, %r14
    # mov %si, (%r13)
    # mov %si, (%r14)
    # mfence

dump:
    nop
    nop
    .rep 10
    imul $1, %rdi
    .endr
    shl $8, %rax
    sahf                   # save flags
    
    jc target

.global safe
safe:
    nop
    nop
    # mov $0x61, %rax
    # shl $12, %rax
    # mov (%r12, %rax, 1), %r13b

    .rep 16
    nop
    .endr

    lahf
    popf
    leave
    ret


.global target
target:
    nop
    nop
    # mov $0x62, %rax
    # shl $12, %rax
    # mov (%r12, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    leave
    ret                 # c3


