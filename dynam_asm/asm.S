/* #include "asm_const.h" */
.section .text
.globl bar
.type bar, @function
bar:
  push %rbp
  mov %rsp, %rbp
  mov $42, %eax
  pop %rbp
  /* asm code with proper stack manipulation for C calling conventions */
  ret
