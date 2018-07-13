    .text
    .global blkcpy
    .type	blkcpy, @function

blkcpy:
    movq   %rdx, %rcx
    cld
    rep    movsb
    ret
