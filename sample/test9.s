	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	xorl	%ebx, %ebx
	.p2align	4, 0x90
LBB0_1:                                 ## %loop
                                        ## =>This Inner Loop Header: Depth=1
	incl	%ebx
	cmpl	$5, %ebx
	jl	LBB0_1
## %bb.2:                               ## %afterloop
	movl	%ebx, %edi
	callq	_printnum
	movl	%ebx, %eax
	popq	%rbx
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_msleep                 ## -- Begin function msleep
	.p2align	4, 0x90
_msleep:                                ## @msleep
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$10, %edi
	callq	_usleep
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_printnum               ## -- Begin function printnum
	.p2align	4, 0x90
_printnum:                              ## @printnum
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, %esi
	leaq	L_.str(%rip), %rdi
	xorl	%eax, %eax
	popq	%rbp
	jmp	_printf                 ## TAILCALL
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4               ## -- Begin function usclock
LCPI3_0:
	.long	1127219200              ## 0x43300000
	.long	1160773632              ## 0x45300000
	.long	0                       ## 0x0
	.long	0                       ## 0x0
LCPI3_1:
	.quad	4841369599423283200     ## double 4503599627370496
	.quad	4985484787499139072     ## double 1.9342813113834067E+25
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3
LCPI3_2:
	.quad	4696837146684686336     ## double 1.0E+6
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_usclock
	.p2align	4, 0x90
_usclock:                               ## @usclock
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	callq	_clock
	movq	%rax, %xmm0
	punpckldq	LCPI3_0(%rip), %xmm0 ## xmm0 = xmm0[0],mem[0],xmm0[1],mem[1]
	subpd	LCPI3_1(%rip), %xmm0
	haddpd	%xmm0, %xmm0
	movsd	LCPI3_2(%rip), %xmm1    ## xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm0
	divsd	%xmm1, %xmm0
	cvttsd2si	%xmm0, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%d\n"


.subsections_via_symbols
