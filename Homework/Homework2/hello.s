	.file	"hello.c"
	.section	.rodata
.LC0:
	.string	"r+a"
.LC1:
	.string	"Error opening file.\n"
	.align 8
.LC2:
	.string	"File %s opened and reads as follows: \n"
.LC3:
	.string	"%s"
.LC4:
	.string	"Writing to file: %s"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movabsq	$8392585648223840614, %rax
	movq	%rax, -32(%rbp)
	movb	$0, -24(%rbp)
	leaq	-32(%rbp), %rax
	movl	$.LC0, %esi
	movq	%rax, %rdi
	call	fopen
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L2
	movl	$.LC1, %edi
	call	perror
	movl	$1, %edi
	call	exit
.L2:
	leaq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	movq	$0, -40(%rbp)
	movq	$0, -48(%rbp)
	jmp	.L3
.L4:
	movq	-40(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
.L3:
	movq	-8(%rbp), %rdx
	leaq	-48(%rbp), %rcx
	leaq	-40(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	getline
	movq	%rax, -16(%rbp)
	cmpq	$-1, -16(%rbp)
	jne	.L4
	movabsq	$8031079698440938824, %rax
	movq	%rax, -80(%rbp)
	movabsq	$8029764343382898976, %rax
	movq	%rax, -72(%rbp)
	movw	$2593, -64(%rbp)
	movb	$0, -62(%rbp)
	leaq	-80(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC4, %edi
	movl	$0, %eax
	call	printf
	leaq	-80(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fclose
	movq	-40(%rbp), %rax
	testq	%rax, %rax
	je	.L5
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	free
.L5:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.3 20140911 (Red Hat 4.8.3-9)"
	.section	.note.GNU-stack,"",@progbits
