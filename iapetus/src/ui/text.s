	.file	"text.c"
	.text
	.text
	.align 1
	.align 4
	.global	_VdpPrintText
	.type	_VdpPrintText, @function
_VdpPrintText:
	mov.l	r8,@-r15
	mova	.L7,r0
	mov.l	r9,@-r15
	mov	r5,r9
	mov.l	r10,@-r15
	mov	r4,r10
	mov.l	r11,@-r15
	mov.l	r12,@-r15
	mov.l	r13,@-r15
	mov.l	r14,@-r15
	sts.l	pr,@-r15
	add	#-8,r15
	mov	r15,r14
	mov.l	@(40,r14),r13
	mov.l	.L7,r12
	mov	r13,r4
	mov.l	.L8,r1
	add	r0,r12
	mov.l	r6,@(4,r14)
	bsrf	r1
.LPCS0:
	mov.l	r7,@r14
	cmp/pl	r0
	bf.s	.L1
	mov	r0,r11
	mov	#0,r8
	.align 2
.L3:
	mov	r8,r0
	mov.b	@(r0,r13),r1
	add	#-4,r15
	mov.l	@(20,r10),r0
	mov	r9,r5
	mov.l	r1,@r15
	mov	r10,r4
	mov.l	@(4,r14),r6
	add	#1,r8
	jsr	@r0
	mov.l	@r14,r7
	cmp/gt	r8,r11
	add	r0,r9
	bt.s	.L3
	add	#4,r15
.L1:
	add	#8,r14
	mov	r14,r15
	lds.l   @r15+,pr
	mov.l   @r15+,r14
	mov.l   @r15+,r13
	mov.l   @r15+,r12
	mov.l   @r15+,r11
	mov.l   @r15+,r10
	mov.l   @r15+,r9
	mov.l   @r15+,r8
	rts	
	nop
.L9:
	.align 2
.L7:
	.long	_GLOBAL_OFFSET_TABLE_
.L8:
	.long	_strlen@PLT-(.LPCS0+2-.)
	.size	_VdpPrintText, .-_VdpPrintText
	.align 1
	.align 4
	.global	_VdpPrintf
	.type	_VdpPrintf, @function
_VdpPrintf:
	mov.l	r8,@-r15
	mov	r6,r8
	mov.l	r9,@-r15
	mova	.L13,r0
	mov.l	r10,@-r15
	mov	r5,r9
	mov.l	r11,@-r15
	mov	r4,r10
	mov.l	r12,@-r15
	mov.l	r14,@-r15
	mov.w	.L18,r1
	sts.l	pr,@-r15
	mov.l	.L13,r12
	sub	r1,r15
	mov.w	.L14,r6
	mov	r15,r14
	add	r0,r12
	mov.w	.L15,r0
	add	r14,r6
	mov.l	.L16,r1
	mov	r14,r11
	mov.l	@r6+,r5
	add	#4,r11
	mov.l	r6,@(r0,r14)
	mov	r11,r4 ! string
	bsrf	r1 ! _vsprintf
.LPCS1:
	mov.l	r7,@r14
	mov.l	.L17,r1
	add	#-4,r15
	mov.l	@r14,r7
	mov	r10,r4
	mov.l	r11,@r15
	mov	r9,r5
	bsrf	r1 ! VdpPrintText
.LPCS2:
	mov	r8,r6
	add	#4,r15
	mov.w	.L18,r7
	add	r7,r14
	mov	r14,r15
	lds.l   @r15+,pr
	mov.l   @r15+,r14
	mov.l   @r15+,r12
	mov.l   @r15+,r11
	mov.l   @r15+,r10
	mov.l   @r15+,r9
	mov.l   @r15+,r8
	rts	
	nop
	.align 1
.L18:
	.short	264
.L14:
	.short	292
.L15:
	.short	260
.L19:
	.align 2
.L13:
	.long	_GLOBAL_OFFSET_TABLE_
.L16:
	.long	_vsprintf@PLT-(.LPCS1+2-.)
.L17:
	.long	_VdpPrintText@PLT-(.LPCS2+2-.)
	.size	_VdpPrintf, .-_VdpPrintf
	.align 1
	.align 4
	.global	_VdpClearScreen
	.type	_VdpClearScreen, @function
_VdpClearScreen:
	mov.l	r12,@-r15
	mova	.L27,r0
	mov.l	.L27,r12
	mov.l	r14,@-r15
	add	r0,r12
	mov.l	@(24,r4),r0
	sts.l	pr,@-r15
	cmp/eq	#7,r0
	bt.s	.L25
	mov	r15,r14
	mov.l	@(16,r4),r2
	mov	#0,r3
	mov.l	.L28,r1
	.align 2
.L23:
	mov.l	r3,@r2
	dt	r1
	bf.s	.L23
	add	#4,r2
.L20:
	mov	r14,r15
	lds.l   @r15+,pr
	mov.l   @r15+,r14
	mov.l   @r15+,r12
	rts	
	nop
	.align 4
.L25:
	mov.l	.L29,r1
	bsrf	r1
.LPCS3:
	nop
	mov.l	.L30,r1
	bsrf	r1
.LPCS4:
	nop
	bra	.L20
	nop
.L31:
	.align 2
.L27:
	.long	_GLOBAL_OFFSET_TABLE_
.L28:
	.long	32768
.L29:
	.long	_VdpStartDrawList@PLT-(.LPCS3+2-.)
.L30:
	.long	_VdpEndDrawList@PLT-(.LPCS4+2-.)
	.size	_VdpClearScreen, .-_VdpClearScreen
	.ident	"GCC: (GNU) 4.5-GNUSH_v10.02"
