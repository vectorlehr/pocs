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
vic:
    pushf
    lea probe_array, %r12
    lea mem, %r13
    lea safe1, %r14
    # mov %si, (%r13)
    mov %si, (%r14)
    # mfence
    .rep 2
    nop
    .endr

dump:
    nop
    nop
    xor %rax, %rax
    .rep 10
    imul $1, %rdi
    .endr
    mov %rdi, %rax
    push %rax
    pop %rax
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
    nop
    mov $0x62, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    ret                 # c3


.global train_branch
.align 0x1000
train_branch:

    push %rbp
    mov %rsp, %rbp
    pushf

    lea probe_array, %r15

    lea safe1, %r14
    mov %si, (%r14)

    lea target_normal, %r14
    mov %si, (%r14)

    # mov %rsi, %rcx
    xor %rax, %rax
    .rep 10
    imul $1, %rdi
    .endr
    mov %rdi, %rax
    push %ax
    popf
    cpuid

    # shl $8, %rax
    # sahf                   # save flags

    jc target_normal

safe1:
    nop
    nop
    mov $0x30, %rax
    
    and $0xff, %rax
    shlq $12, %rax
    mov (%r15,%rax,1), %rax
    popf
    mov %rbp, %rsp
    pop %rbp
    ret

target_normal:
    nop
    nop
    mov $0xf3, %rax
    and $0xff, %rax
    shlq $12, %rax
    mov (%r15,%rax,1), %rax
    popf
    mov %rbp, %rsp
    pop %rbp
    ret