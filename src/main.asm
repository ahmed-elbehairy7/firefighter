	.file	"main.cpp"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
	.section	.text.startup,"ax",@progbits
.global	main
	.type	main, @function
main:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ldi r24,lo8(62)
	out 0x4,r24
	sts 197,__zero_reg__
	ldi r24,lo8(103)
	sts 196,r24
	ldi r24,lo8(24)
	sts 193,r24
	ldi r24,lo8(6)
	sts 194,r24
	in r24,__SREG__
	ori r24,lo8(-128)
	out __SREG__,r24
	lds r24,104
	ori r24,lo8(1)
	sts 104,r24
	lds r24,107
	ori r24,lo8(1)
	sts 107,r24
	lds r24,193
	ori r24,lo8(-128)
	sts 193,r24
.L2:
	rjmp .L2
	.size	main, .-main
	.text
.global	__vector_3
	.type	__vector_3, @function
__vector_3:
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 3 */
.L__stack_usage = 3
	sbi 0x5,0
/* #APP */
 ;  128 "./main.cpp" 1
	nop
 ;  0 "" 2
/* #NOAPP */
	sbis 0x3,0
	rjmp .L4
	sbi 0x5,1
.L3:
/* epilogue start */
	pop r0
	out __SREG__,r0
	pop r0
	pop r1
	reti
.L4:
	cbi 0x5,1
	rjmp .L3
	.size	__vector_3, .-__vector_3
.global	__vector_18
	.type	__vector_18, @function
__vector_18:
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r24
	push r28
	push r29
	push __zero_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: Signal */
/* frame size = 1 */
/* stack size = 7 */
.L__stack_usage = 7
	lds r24,198
	andi r24,lo8(-65)
	out 0x5,r24
.L8:
	lds r24,192
	sbrs r24,7
	rjmp .L6
	lds r24,198
	std Y+1,r24
	rjmp .L8
.L6:
/* epilogue start */
	pop __tmp_reg__
	pop r29
	pop r28
	pop r24
	pop r0
	out __SREG__,r0
	pop r0
	pop r1
	reti
	.size	__vector_18, .-__vector_18
	.ident	"GCC: (AVR_8_bit_GNU_Toolchain_3.7.0_1796) 7.3.0"
