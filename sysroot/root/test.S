.globl _start
.text
_start:
    la a0, string
    li a7, 4
    ecall
    li a7, 7
    ecall
.data
string: .string "Hello from RARSJS on SalernOS!\n"
