	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 6	@ Tag_CPU_arch
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
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"llvm-link"
	.globl	msleep                  @ -- Begin function msleep
	.p2align	2
	.type	msleep,%function
	.code	32                      @ @msleep
msleep:
	.fnstart
@ %bb.0:
	.save	{r11, lr}
	push	{r11, lr}
	.setfp	r11, sp
	mov	r11, sp
	mov	r0, #10
	bl	usleep
	mov	r0, #0
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end0:
	.size	msleep, .Lfunc_end0-msleep
	.fnend
                                        @ -- End function
	.globl	printnum                @ -- Begin function printnum
	.p2align	2
	.type	printnum,%function
	.code	32                      @ @printnum
printnum:
	.fnstart
@ %bb.0:
	mov	r1, r0
	ldr	r0, .LCPI1_0
	b	printf
	.p2align	2
@ %bb.1:
.LCPI1_0:
	.long	.L.str
.Lfunc_end1:
	.size	printnum, .Lfunc_end1-printnum
	.fnend
                                        @ -- End function
	.globl	usclock                 @ -- Begin function usclock
	.p2align	2
	.type	usclock,%function
	.code	32                      @ @usclock
usclock:
	.fnstart
@ %bb.0:
	.save	{r4, r10, r11, lr}
	push	{r4, r10, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	bl	clock
	bl	__aeabi_ul2d
	ldr	r4, .LCPI2_0
	mov	r2, #0
	mov	r3, r4
	bl	__aeabi_dmul
	mov	r2, #0
	mov	r3, r4
	bl	__aeabi_ddiv
	bl	__aeabi_d2iz
	pop	{r4, r10, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI2_0:
	.long	1093567616              @ 0x412e8480
.Lfunc_end2:
	.size	usclock, .Lfunc_end2-usclock
	.fnend
                                        @ -- End function
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
	ldr	r0, .LCPI3_0
	mov	r1, #130
	bl	open
	ldr	r5, .LCPI3_1
	cmn	r0, #1
	str	r0, [r5, #8]
	ble	.LBB3_4
@ %bb.1:
	ldm	r5, {r1, r2}
	str	r2, [sp, #20]
	mov	r4, #1
	mov	r3, #0
	mov	r2, #4096
	str	r1, [sp, #16]
	mov	r1, #3
	stm	sp, {r1, r4}
	str	r0, [sp, #8]
	mov	r0, #0
	bl	mmap
	cmn	r0, #1
	str	r0, [r5, #12]
	beq	.LBB3_5
@ %bb.2:
	mov	r4, #0
	str	r0, [r5, #16]
.LBB3_3:
	mov	r0, r4
	sub	sp, r11, #8
	pop	{r4, r5, r11, lr}
	mov	pc, lr
.LBB3_4:
	ldr	r0, .LCPI3_3
	bl	perror
	mov	r4, #1
	b	.LBB3_3
.LBB3_5:
	ldr	r0, .LCPI3_2
	bl	perror
	b	.LBB3_3
	.p2align	2
@ %bb.6:
.LCPI3_0:
	.long	.L.str.1
.LCPI3_1:
	.long	gpio
.LCPI3_2:
	.long	.L.str.3
.LCPI3_3:
	.long	.L.str.2
.Lfunc_end3:
	.size	MapGPIO, .Lfunc_end3-MapGPIO
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
	ldr	r4, .LCPI4_0
	mov	r2, #4096
	mov	r3, #0
	ldr	r0, [r4, #12]
	bl	munmap
	ldr	r0, [r4, #8]
	pop	{r4, r10, r11, lr}
	b	close
	.p2align	2
@ %bb.1:
.LCPI4_0:
	.long	gpio
.Lfunc_end4:
	.size	GPIOclear, .Lfunc_end4-GPIOclear
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
	ldr	r1, .LCPI5_0
	ldr	r6, .LCPI5_1
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
.LCPI5_0:
	.long	1717986919              @ 0x66666667
.LCPI5_1:
	.long	gpio
.Lfunc_end5:
	.size	BlinkLED, .Lfunc_end5-BlinkLED
	.fnend
                                        @ -- End function
	.globl	GPIOsetup               @ -- Begin function GPIOsetup
	.p2align	2
	.type	GPIOsetup,%function
	.code	32                      @ @GPIOsetup
GPIOsetup:
	.fnstart
@ %bb.0:
	.save	{r4, r5, r11, lr}
	push	{r4, r5, r11, lr}
	.setfp	r11, sp, #8
	add	r11, sp, #8
	.pad	#24
	sub	sp, sp, #24
	ldr	r0, .LCPI6_0
	mov	r1, #130
	bl	open
	ldr	r5, .LCPI6_1
	cmn	r0, #1
	str	r0, [r5, #8]
	ble	.LBB6_4
@ %bb.1:
	ldm	r5, {r1, r2}
	str	r2, [sp, #20]
	str	r0, [sp, #8]
	mov	r0, #0
	mov	r3, #0
	mov	r4, #0
	mov	r2, #3
	str	r2, [sp]
	mov	r2, #4096
	str	r1, [sp, #16]
	mov	r1, #1
	str	r1, [sp, #4]
	bl	mmap
	cmn	r0, #1
	str	r0, [r5, #12]
	beq	.LBB6_5
@ %bb.2:
	str	r0, [r5, #16]
.LBB6_3:
	mov	r0, r4
	sub	sp, r11, #8
	pop	{r4, r5, r11, lr}
	mov	pc, lr
.LBB6_4:
	ldr	r0, .LCPI6_3
	b	.LBB6_6
.LBB6_5:
	ldr	r0, .LCPI6_2
.LBB6_6:
	bl	perror
	ldr	r0, .LCPI6_4
	bl	puts
	mov	r4, #1
	b	.LBB6_3
	.p2align	2
@ %bb.7:
.LCPI6_0:
	.long	.L.str.1
.LCPI6_1:
	.long	gpio
.LCPI6_2:
	.long	.L.str.3
.LCPI6_3:
	.long	.L.str.2
.LCPI6_4:
	.long	.Lstr
.Lfunc_end6:
	.size	GPIOsetup, .Lfunc_end6-GPIOsetup
	.fnend
                                        @ -- End function
	.globl	main                    @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                      @ @main
main:
	.fnstart
@ %bb.0:                                @ %entry
	.save	{r4, lr}
	push	{r4, lr}
	mov	r4, #0
.LBB7_1:                                @ %loop
                                        @ =>This Inner Loop Header: Depth=1
	add	r4, r4, #1
	cmp	r4, #5
	blt	.LBB7_1
@ %bb.2:                                @ %afterloop
	mov	r0, r4
	bl	printnum
	mov	r0, r4
	pop	{r4, pc}
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
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
	.asciz	"%d\n"
	.size	.L.str, 4

	.type	.L.str.1,%object        @ @.str.1
.L.str.1:
	.asciz	"/dev/mem"
	.size	.L.str.1, 9

	.type	.L.str.2,%object        @ @.str.2
.L.str.2:
	.asciz	"Failed to open /dev/mem, try change permission."
	.size	.L.str.2, 48

	.type	.L.str.3,%object        @ @.str.3
.L.str.3:
	.asciz	"mmap"
	.size	.L.str.3, 5

	.type	.Lstr,%object           @ @str
	.section	.rodata.str1.16,"aMS",%progbits,1
	.p2align	4
.Lstr:
	.asciz	"Failed to blink LED."
	.size	.Lstr, 21


	.ident	"Apple LLVM version 10.0.0 (clang-1000.10.44.4)"
	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1	@ Tag_ABI_optimization_goals
