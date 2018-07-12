    .text
    .global lut
    .type	lut, @function

lut:
    pushq   %rbx
    leaq    table(%rip), %rbx
    movl    %edi, %eax
    xlat
    popq    %rbx
    ret
