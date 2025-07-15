.section .text
.global __mlibc_restorer
__mlibc_restorer:
	mov $32, %rax
	int $0x80
.section .note.GNU-stack,"",%progbits

