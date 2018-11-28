	.file	"lib2.ll"
	.text
	.globl	msleep
	.align	16, 0x90
	.type	msleep,@function
msleep:                                 # @msleep
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	%edi, 4(%rsp)
	movl	$10, %edi
	callq	usleep
	xorl	%eax, %eax
	popq	%rdx
	ret
.Ltmp2:
	.size	msleep, .Ltmp2-msleep
	.cfi_endproc

	.globl	printnum
	.align	16, 0x90
	.type	printnum,@function
printnum:                               # @printnum
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp4:
	.cfi_def_cfa_offset 16
	movl	%edi, %eax
	movl	%eax, 4(%rsp)
	movl	$.L.str, %edi
	movl	%eax, %esi
	xorb	%al, %al
	callq	printf
	popq	%rdx
	ret
.Ltmp5:
	.size	printnum, .Ltmp5-printnum
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	 "%d\n"
	.size	.L.str, 4


	.section	".note.GNU-stack","",@progbits
