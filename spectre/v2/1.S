.data

.global probe_array
probe_array:
    .space 4096 * 256, 0x0


.global print_str1
print_str1:
    .asciz "f1\n"

.global print_str2
print_str2:
    .asciz "f2\n"


.text

.extern puts


.global f1
.align 0x1000
f1:
    # push %rbp
    # mov %rsp, %rbp

    lea probe_array, %r12
    mov $0x6c, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b

    # lea print_str1, %rdi
    # call puts

    # pop %rbp
    ret

.global f2
.align 0x1000
f2:
    # push %rbp
    # mov %rsp, %rbp

    lea probe_array, %r12
    mov $0x6d, %rax
    shl $12, %rax
    mov (%r12, %rax, 1), %r13b

    # lea print_str2, %rdi
    # call puts

    # pop %rbp
    ret

.global train_btb
.align 0x1000
train_btb:
    # push %rbp
    # mov %rsp, %rbp

    nop

    .rep 1000
    imul $1, %rdi
    # mul %rdi, %rdi, $1
    .endr

    # clflush 0(%rdi)

    mov %rdi, %rax

    # call *%rax
    jmp *%rax

    # pop %rbp
    ret
