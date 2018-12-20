	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	pushq	%rax
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	xorl	%ebx, %ebx
	movb	$1, %bpl
	.p2align	4, 0x90
LBB0_1:                                 ## %loop
                                        ## =>This Inner Loop Header: Depth=1
	incl	%ebx
	movl	%ebx, %edi
	callq	_printnum
	testb	%bpl, %bpl
	jne	LBB0_1
## %bb.2:                               ## %afterloop
	callq	_usclock
	callq	_printnum
	movl	%ebx, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
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
	.globl	_MapGPIO                ## -- Begin function MapGPIO
	.p2align	4, 0x90
_MapGPIO:                               ## @MapGPIO
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
	.cfi_offset %rbx, -24
	leaq	L_.str.1(%rip), %rdi
	movl	$130, %esi
	xorl	%eax, %eax
	callq	_open
	movl	%eax, _gpio+8(%rip)
	testl	%eax, %eax
	js	LBB4_1
## %bb.2:
	movq	_gpio(%rip), %r9
	movl	$1, %ebx
	movl	$4096, %esi             ## imm = 0x1000
	movl	$0, %edi
	movl	$3, %edx
	movl	$1, %ecx
	movl	%eax, %r8d
	callq	_mmap
	movq	%rax, _gpio+16(%rip)
	cmpq	$-1, %rax
	je	LBB4_3
## %bb.4:
	movq	%rax, _gpio+24(%rip)
	xorl	%ebx, %ebx
LBB4_5:
	movl	%ebx, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
LBB4_1:
	leaq	L_.str.2(%rip), %rdi
	callq	_perror
	movl	$1, %ebx
	jmp	LBB4_5
LBB4_3:
	leaq	L_.str.3(%rip), %rdi
	callq	_perror
	jmp	LBB4_5
	.cfi_endproc
                                        ## -- End function
	.globl	_GPIOclear              ## -- Begin function GPIOclear
	.p2align	4, 0x90
_GPIOclear:                             ## @GPIOclear
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	_gpio+16(%rip), %rdi
	movl	$4096, %esi             ## imm = 0x1000
	callq	_munmap
	movl	_gpio+8(%rip), %edi
	popq	%rbp
	jmp	_close                  ## TAILCALL
	.cfi_endproc
                                        ## -- End function
	.globl	_BlinkLED               ## -- Begin function BlinkLED
	.p2align	4, 0x90
_BlinkLED:                              ## @BlinkLED
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movslq	%edi, %rax
	imulq	$1717986919, %rax, %rdx ## imm = 0x66666667
	movq	%rdx, %rcx
	shrq	$63, %rcx
	sarq	$34, %rdx
	addl	%ecx, %edx
	leal	(%rdx,%rdx), %ecx
	leal	(%rcx,%rcx,4), %ecx
	movl	%eax, %esi
	subl	%ecx, %esi
	leal	(%rsi,%rsi,2), %ecx
	movl	$1, %esi
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %esi
	movl	$1, %ebx
	movq	_gpio+24(%rip), %rdi
	movslq	%edx, %r14
	movl	%esi, (%rdi,%r14,4)
	movl	%eax, %ecx
	shll	%cl, %ebx
	movl	%ebx, 28(%rdi)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 40(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 28(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 40(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 28(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 40(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 28(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	%ebx, 40(%rax)
	movl	$500000, %edi           ## imm = 0x7A120
	callq	_usleep
	movq	_gpio+24(%rip), %rax
	movl	$0, (%rax,%r14,4)
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_GPIOsetup              ## -- Begin function GPIOsetup
	.p2align	4, 0x90
_GPIOsetup:                             ## @GPIOsetup
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	pushq	%rax
	.cfi_offset %rbx, -24
	leaq	L_.str.1(%rip), %rdi
	movl	$130, %esi
	xorl	%eax, %eax
	callq	_open
	movl	%eax, _gpio+8(%rip)
	testl	%eax, %eax
	js	LBB7_1
## %bb.2:
	movq	_gpio(%rip), %r9
	xorl	%ebx, %ebx
	movl	$4096, %esi             ## imm = 0x1000
	movl	$0, %edi
	movl	$3, %edx
	movl	$1, %ecx
	movl	%eax, %r8d
	callq	_mmap
	movq	%rax, _gpio+16(%rip)
	cmpq	$-1, %rax
	je	LBB7_3
## %bb.6:
	movq	%rax, _gpio+24(%rip)
LBB7_5:
	movl	%ebx, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
LBB7_1:
	leaq	L_.str.2(%rip), %rdi
	jmp	LBB7_4
LBB7_3:
	leaq	L_.str.3(%rip), %rdi
LBB7_4:
	callq	_perror
	leaq	L_str(%rip), %rdi
	callq	_puts
	movl	$1, %ebx
	jmp	LBB7_5
	.cfi_endproc
                                        ## -- End function
	.section	__DATA,__data
	.globl	_gpio                   ## @gpio
	.p2align	3
_gpio:
	.quad	1059061760              ## 0x3f200000
	.long	0                       ## 0x0
	.space	4
	.quad	0
	.quad	0

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%d\n"

L_.str.1:                               ## @.str.1
	.asciz	"/dev/mem"

L_.str.2:                               ## @.str.2
	.asciz	"Failed to open /dev/mem, try change permission."

L_.str.3:                               ## @.str.3
	.asciz	"mmap"

	.p2align	4               ## @str
L_str:
	.asciz	"Failed to blink LED."


.subsections_via_symbols
