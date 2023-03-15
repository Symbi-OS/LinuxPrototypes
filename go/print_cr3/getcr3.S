/* put this in a function called my_asm in the .text section */    

    .text
    .globl main.getcr3
    .globl main.get42

main.getcr3:
    mov %cr3, %rax
    ret
    
main.get42:
    mov $42, %rax
    ret

