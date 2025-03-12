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
    