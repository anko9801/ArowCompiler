	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
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
	leaq	L_.str(%rip), %rdi
	movl	$130, %esi
	xorl	%eax, %eax
	callq	_open
	movl	%eax, _gpio+8(%rip)
	testl	%eax, %eax
	js	LBB0_1
## %bb.2:
	movq	$1059061760, _gpio(%rip) ## imm = 0x3F200000
	leaq	L_.str.2(%rip), %rdi
	movl	$8, %esi
	movl	$32, %edx
	xorl	%eax, %eax
	callq	_printf
	movq	_gpio(%rip), %rsi
	leaq	L_.str.3(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	movl	_gpio+8(%rip), %esi
	leaq	L_.str.4(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	movl	_gpio+8(%rip), %r8d
	movq	_gpio(%rip), %r9
	movl	$1, %ebx
	movl	$4096, %esi             ## imm = 0x1000
	movl	$0, %edi
	movl	$3, %edx
	movl	$1, %ecx
	callq	_mmap
	movq	%rax, _gpio+16(%rip)
	cmpq	$-1, %rax
	je	LBB0_3
## %bb.4:
	movq	%rax, _gpio+24(%rip)
	xorl	%ebx, %ebx
LBB0_5:
	movl	%ebx, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
LBB0_1:
	leaq	L_.str.1(%rip), %rdi
	callq	_perror
	movl	$1, %ebx
	jmp	LBB0_5
LBB0_3:
	leaq	L_.str.5(%rip), %rdi
	callq	_perror
	jmp	LBB0_5
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
	callq	_MapGPIO
	movl	%eax, %ebx
	testl	%eax, %eax
	je	LBB3_2
## %bb.1:
	leaq	L_str(%rip), %rdi
	callq	_puts
LBB3_2:
	movl	%ebx, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	callq	_MapGPIO
	testl	%eax, %eax
	je	LBB4_2
## %bb.1:
	leaq	L_str(%rip), %rdi
	callq	_puts
LBB4_2:
	movl	$23, %edi
	callq	_BlinkLED
	movq	_gpio+16(%rip), %rdi
	movl	$4096, %esi             ## imm = 0x1000
	callq	_munmap
	movl	_gpio+8(%rip), %edi
	callq	_close
	xorl	%eax, %eax
	popq	%rbp
	retq
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
	.asciz	"/dev/mem"

L_.str.1:                               ## @.str.1
	.asciz	"Failed to open /dev/mem, try change permission."

L_.str.2:                               ## @.str.2
	.asciz	"void:%x rpi:%x\n"

L_.str.3:                               ## @.str.3
	.asciz	"base:%x\n"

L_.str.4:                               ## @.str.4
	.asciz	"memory:%x\n"

L_.str.5:                               ## @.str.5
	.asciz	"mmap"

	.p2align	4               ## @str
L_str:
	.asciz	"Failed to blink LED."


.subsections_via_symbols
