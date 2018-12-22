	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 14	@ Tag_CPU_arch
	.eabi_attribute	7, 65	@ Tag_CPU_arch_profile
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	9, 2	@ Tag_THUMB_ISA_use
	.fpu	crypto-neon-fp-armv8
	.eabi_attribute	12, 3	@ Tag_Advanced_SIMD_arch
	.eabi_attribute	36, 1	@ Tag_FP_HP_extension
	.eabi_attribute	42, 1	@ Tag_MPextension_use
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	68, 3	@ Tag_Virtualization_use
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	28, 1	@ Tag_ABI_VFP_args
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"blink.c"
	.globl	MapGPIO                 @ -- Begin function MapGPIO
	.p2align	2
	.type	MapGPIO,%function
	.code	32                      @ @MapGPIO
MapGPIO:
	.fnstart
@ %bb.0:
	.save	{r4, r5, r11, lr}
	push	{r4, r5, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	.pad	#24
	sub	sp, sp, #24
	ldr	r0, .LCPI0_0
	mov	r1, #130
	bl	open
	ldr	r5, .LCPI0_1
	cmn	r0, #1
	str	r0, [r5, #8]
	ble	.LBB0_4
@ %bb.1:
	ldr	r0, .LCPI0_2
	mov	r2, #8
	mov	r3, #0
	bl	printf
	ldr	r0, .LCPI0_3
	mov	r2, #32
	mov	r3, #0
	bl	printf
	ldm	r5, {r2, r3}
	ldr	r0, .LCPI0_4
	bl	printf
	ldr	r1, [r5, #8]
	ldr	r0, .LCPI0_5
	bl	printf
	ldm	r5, {r0, r1, r2}
	str	r1, [sp, #20]
	mov	r4, #1
	mov	r3, #0
	str	r0, [sp, #16]
	mov	r0, #3
	stm	sp, {r0, r4}
	str	r2, [sp, #8]
	mov	r0, #0
	mov	r2, #4096
	bl	mmap
	cmn	r0, #1
	str	r0, [r5, #12]
	beq	.LBB0_5
@ %bb.2:
	mov	r4, #0
	str	r0, [r5, #16]
.LBB0_3:
	mov	r0, r4
	sub	sp, r11, #8
	pop	{r4, r5, r11, lr}
	mov	pc, lr
.LBB0_4:
	ldr	r0, .LCPI0_7
	bl	perror
	mov	r4, #1
	b	.LBB0_3
.LBB0_5:
	ldr	r0, .LCPI0_6
	bl	perror
	b	.LBB0_3
	.p2align	2
@ %bb.6:
.LCPI0_0:
	.long	.L.str
.LCPI0_1:
	.long	gpio
.LCPI0_2:
	.long	.L.str.2
.LCPI0_3:
	.long	.L.str.3
.LCPI0_4:
	.long	.L.str.4
.LCPI0_5:
	.long	.L.str.5
.LCPI0_6:
	.long	.L.str.6
.LCPI0_7:
	.long	.L.str.1
.Lfunc_end0:
	.size	MapGPIO, .Lfunc_end0-MapGPIO
	.fnend
                                        @ -- End function
	.globl	GPIOclear               @ -- Begin function GPIOclear
	.p2align	2
	.type	GPIOclear,%function
	.code	32                      @ @GPIOclear
GPIOclear:
	.fnstart
@ %bb.0:
	.save	{r4, r10, r11, lr}
	push	{r4, r10, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	ldr	r4, .LCPI1_0
	mov	r2, #4096
	mov	r3, #0
	ldr	r0, [r4, #12]
	bl	munmap
	ldr	r0, [r4, #8]
	pop	{r4, r10, r11, lr}
	b	close
	.p2align	2
@ %bb.1:
.LCPI1_0:
	.long	gpio
.Lfunc_end1:
	.size	GPIOclear, .Lfunc_end1-GPIOclear
	.fnend
                                        @ -- End function
	.globl	BlinkLED                @ -- Begin function BlinkLED
	.p2align	2
	.type	BlinkLED,%function
	.code	32                      @ @BlinkLED
BlinkLED:
	.fnstart
@ %bb.0:
	.save	{r4, r5, r6, r7, r11, lr}
	push	{r4, r5, r6, r7, r11, lr}
	.setfp	r11, sp, #16
	add	r11, sp, #16
	ldr	r1, .LCPI2_0
	ldr	r6, .LCPI2_1
	mov	r4, #288
	orr	r4, r4, #499712
	smull	r2, r3, r0, r1
	asr	r1, r3, #2
	mov	r2, #1
	add	r5, r1, r3, lsr #31
	ldr	r3, [r6, #16]
	lsl	r7, r2, r0
	add	r1, r5, r5, lsl #2
	sub	r1, r0, r1, lsl #1
	mov	r0, r4
	add	r1, r1, r1, lsl #1
	lsl	r1, r2, r1
	str	r1, [r3, r5, lsl #2]
	str	r7, [r3, #28]
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #40]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #28]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #40]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #28]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #40]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #28]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	str	r7, [r0, #40]
	mov	r0, r4
	bl	usleep
	ldr	r0, [r6, #16]
	mov	r1, #0
	str	r1, [r0, r5, lsl #2]
	pop	{r4, r5, r6, r7, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI2_0:
	.long	1717986919              @ 0x66666667
.LCPI2_1:
	.long	gpio
.Lfunc_end2:
	.size	BlinkLED, .Lfunc_end2-BlinkLED
	.fnend
                                        @ -- End function
	.globl	GPIOsetup               @ -- Begin function GPIOsetup
	.p2align	2
	.type	GPIOsetup,%function
	.code	32                      @ @GPIOsetup
GPIOsetup:
	.fnstart
@ %bb.0:
	.save	{r4, r10, r11, lr}
	push	{r4, r10, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	bl	MapGPIO
	mov	r4, r0
	cmp	r0, #0
	beq	.LBB3_2
@ %bb.1:
	ldr	r0, .LCPI3_0
	bl	puts
.LBB3_2:
	mov	r0, r4
	pop	{r4, r10, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI3_0:
	.long	.Lstr
.Lfunc_end3:
	.size	GPIOsetup, .Lfunc_end3-GPIOsetup
	.fnend
                                        @ -- End function
	.globl	main                    @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                      @ @main
main:
	.fnstart
@ %bb.0:
	.save	{r4, r10, r11, lr}
	push	{r4, r10, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	bl	MapGPIO
	cmp	r0, #0
	beq	.LBB4_2
@ %bb.1:
	ldr	r0, .LCPI4_0
	bl	puts
.LBB4_2:
	mov	r0, #23
	bl	BlinkLED
	ldr	r4, .LCPI4_1
	mov	r2, #4096
	mov	r3, #0
	ldr	r0, [r4, #12]
	bl	munmap
	ldr	r0, [r4, #8]
	bl	close
	mov	r0, #0
	pop	{r4, r10, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI4_0:
	.long	.Lstr
.LCPI4_1:
	.long	gpio
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
	.fnend
                                        @ -- End function
	.type	gpio,%object            @ @gpio
	.data
	.globl	gpio
	.p2align	3
gpio:
	.long	1059061760              @ 0x3f200000
	.long	0
	.long	0                       @ 0x0
	.long	0
	.long	0
	.zero	4
	.size	gpio, 24

	.type	.L.str,%object          @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"/dev/mem"
	.size	.L.str, 9

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"Failed to open /dev/mem, try change permission."
	.size	.L.str.1, 48

	.type	.L.str.2,%object        @ @.str.2
.L.str.2:
	.asciz	"void:%x\n"
	.size	.L.str.2, 9

	.type	.L.str.3,%object        @ @.str.3
.L.str.3:
	.asciz	"rpi:%x\n"
	.size	.L.str.3, 8

	.type	.L.str.4,%object        @ @.str.4
.L.str.4:
	.asciz	"base:%x\n"
	.size	.L.str.4, 9

	.type	.L.str.5,%object        @ @.str.5
.L.str.5:
	.asciz	"memory:%x\n"
	.size	.L.str.5, 11

	.type	.L.str.6,%object        @ @.str.6
.L.str.6:
	.asciz	"mmap"
	.size	.L.str.6, 5

	.type	.Lstr,%object           @ @str
	.section	.rodata.str1.16,"aMS",%progbits,1
	.p2align	4
.Lstr:
	.asciz	"Failed to blink LED."
	.size	.Lstr, 21


	.ident	"Apple LLVM version 10.0.0 (clang-1000.10.44.4)"
	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1	@ Tag_ABI_optimization_goals
