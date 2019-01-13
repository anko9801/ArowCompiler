	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 2	@ Tag_CPU_arch
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	9, 1	@ Tag_THUMB_ISA_use
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	28, 1	@ Tag_ABI_VFP_args
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"blink.arow"
	.globl	main                    @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                      @ @main
main:
	.fnstart
@ %bb.0:                                @ %entry
	.save	{r4, lr}
	push	{r4, lr}
	.pad	#16
	sub	sp, sp, #16
	mov	r0, #2
	bl	printnum
	bl	confirm
	mov	r0, #2
	bl	printnum
	mov	r0, #23
	mov	r1, #1
	bl	gpioMode
	mov	r0, #2
	bl	printnum
	mov	r0, #0
	cmp	r0, #5
	str	r0, [sp, #4]
	beq	.LBB0_3
@ %bb.1:                                @ %loop.preheader
	mov	r4, #576
	orr	r4, r4, #999424
.LBB0_2:                                @ %loop
                                        @ =>This Inner Loop Header: Depth=1
	ldr	r0, [sp, #4]
	bl	printnum
	ldr	r0, [sp, #4]
	mov	r1, #1
	add	r0, r0, #1
	str	r0, [sp, #4]
	mov	r0, #23
	bl	gpioWrite
	mov	r0, r4
	bl	wait
	mov	r0, #23
	mov	r1, #0
	bl	gpioWrite
	mov	r0, r4
	bl	wait
	ldr	r0, [sp, #4]
	cmp	r0, #5
	bne	.LBB0_2
.LBB0_3:                                @ %afterloop
	mov	r0, #23
	bl	gpioClear
	bl	Close
	mov	r0, #0
	add	sp, sp, #16
	pop	{r4, lr}
	bx	lr
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.fnend
                                        @ -- End function

	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1	@ Tag_ABI_optimization_goals
