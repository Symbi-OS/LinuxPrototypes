/* put this in a function called my_asm in the .text section */    

    .text
    .globl getcr3
    .globl get42
    .globl my_assembly_function

getcr3:
    mov %cr3, %rax
    ret
    
get42:
    mov $42, %rax
    ret

my_assembly_function:
    mov $0xdeadbeef, %rax
    ret
