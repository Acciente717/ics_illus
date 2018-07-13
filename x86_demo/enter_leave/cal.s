	.file	"cal.c"
	.text
	.globl	cal
	.type	cal, @function
cal:
	enter   $8, $0
	# pushq	%rbp
	# movq	%rsp, %rbp
	# subq    $8, %rsp

	movl	%edi, 4(%rsp)
	movl	%esi, (%rsp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %edx
	subl	%edx, %eax

	leave
	# movq    %rbp, %rsp
	# popq	%rbp
	ret
