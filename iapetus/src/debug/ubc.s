!   Copyright 2007,2013 Theo Berkau
!
!   This file is part of Iapetus.
!
!   Iapetus is free software; you can redistribute it and/or modify
!   it under the terms of the GNU General Public License as published by
!   the Free Software Foundation; either version 2 of the License, or
!   (at your option) any later version.
!
!   Iapetus is distributed in the hope that it will be useful,
!   but WITHOUT ANY WARRANTY; without even the implied warranty of
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!   GNU General Public License for more details.
!
!   You should have received a copy of the GNU General Public License
!   along with Iapetus; if not, write to the Free Software
!   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

.section .text

! ubc_set_code_breakpoint !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.global _ubc_set_code_breakpoint ! R4 = slot, R5 = address, returns IAPETUS_ERR_OK if no problems
_ubc_set_code_breakpoint:
    mov.l   r1, @-r15
    cmp/eq  r4, #0    ! 0 = Breakpoint A, 1 = Breakpoint B
    bf bpb
    ! Breakpoint A

    !set break address
    mov.l   BARA, r1
    mov.l   r5, @r1

    !set address mask
    mov.l   BAMRA, r1
    mov     #0, r0
    mov.l   r0, @r1

    !set break conditions
    mov.l   BBRA, r1
    mov.w   COND, r0 ! Break on CPU cycles, Break only on instruction fetch cycles, break only on read cycles
    mov.w   r0, @r1

    !set break control
    mov.l   BRCR, r1
    mov.w   CTRLA, r0 ! SH7604 mode, break before instruction execution
    mov.w   r0, @r1
    
    mov     #0, r0
    bra     done
    nop
bpb:
    ! Breakpoint B
    !set break address
    mov.l   BARB, r1
    mov.l   r5, @r1

    !set address mask
    mov.l   BAMRA, r1
    mov     #0, r0
    mov.l   r0, @r1

    !set break conditions
    mov.l   BBRB, r1
    mov.w   COND, r0 ! Break on CPU cycles, Break only on instruction fetch cycles, break only on read cycles
    mov.w   r0, @r1

    !set break control
    mov.l   BRCR, r1
!    mov.w   CTRLB, r0 ! FIXME
    mov.w  r0, @r1    
    mov     #0, r0
done:
    mov.l   @r15+, r1   
    rts
    nop
COND:		       .word 0x0054
CTRLA:		       .word 0x1000
CTRLB:		       .word 0x1000
.align 4
BARA:                 .long 0xFFFFFF40
BAMRA:                .long 0xFFFFFF44
BBRA:                 .long 0xFFFFFF48
BARB:                 .long 0xFFFFFF60
BAMRB:                .long 0xFFFFFF64
BBRB:                 .long 0xFFFFFF68
BRCR:                 .long 0xFFFFFF78

! ubc_set_memory_breakpoint !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.global _ubc_set_memory_breakpoint ! R4 = slot, R5 = address, R6 = type, returns IAPETUS_ERR_OK if no problems
_ubc_set_memory_breakpoint:
   rts
   nop

! ubc_int_handler !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.global _ubc_int_handler
_ubc_int_handler:
    ! Save all registers to stack
    stc.l   SR, @-r15
    stc.l   GBR, @-r15
    stc.l   VBR, @-r15
    sts.l   MACH, @-r15
    sts.l   MACL, @-r15
    sts.l   PR, @-r15
    mov.l   r0, @-r15
    mov.l   r1, @-r15
    mov.l   r2, @-r15
    mov.l   r3, @-r15
    mov.l   r4, @-r15
    mov.l   r5, @-r15
    mov.l   r6, @-r15
    mov.l   r7, @-r15
    mov.l   r8, @-r15
    mov.l   r9, @-r15
    mov.l   r10, @-r15
    mov.l   r11, @-r15
    mov.l   r12, @-r15
    mov.l   r13, @-r15
    mov.l   r14, @-r15
    mov.l   r15, @-r15

    ! Copy all registers to work buffer
    mov.l   WORKBUFADDR, r0
    add     #4, r0
    mov.l   r15, r1
    add     #?, r1 ! I need to calculate how much this needs to be adjusted by
copylp:    
    mov.l   @r1+, r2
    mov.l   r2, @r0
    add     #4, r0
!    cmp/ge  r15, r1 !double check to make sure this is right
    bf      copylp

    ! Let PC know we're ready to transfer data
    mov.l   WORKBUFADDR, r0
    mov.l   READY, r1
    mov.l   r1, @r0

    ! Go into endless loop waiting for PC's response
    mov.l   DONE, r1
waitlp:
    !trapa ! this needs work(the idea is it's supposed to latch onto the ar's commlink functions
    mov.l   @r0, r2
    cmp/eq  r2, r1
    bf      waitlp

    ! Restore registers back to their original state
    mov.l   @r15+, r15
    mov.l   @r15+, r14
    mov.l   @r15+, r13
    mov.l   @r15+, r12
    mov.l   @r15+, r11
    mov.l   @r15+, r10
    mov.l   @r15+, r9
    mov.l   @r15+, r8
    mov.l   @r15+, r7
    mov.l   @r15+, r6
    mov.l   @r15+, r5
    mov.l   @r15+, r4
    mov.l   @r15+, r3
    mov.l   @r15+, r2
    mov.l   @r15+, r1
    mov.l   @r15+, r0
    lds.l   @r15+, PR
    lds.l   @r15+, MACL
    lds.l   @r15+, MACH
    ldc.l   @r15+, VBR
    ldc.l   @r15+, GBR
    ldc.l   @r15+, SR
    rte
    nop
.align 4
DONE:                 .long 0x5A5A5A5A
READY:                .long 0xA5A5A5A5
WORKBUFADDR:          .long 0x00000000
