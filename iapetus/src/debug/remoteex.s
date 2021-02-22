!   Copyright 2007 Theo Berkau
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

_RemoteDebugStart:                      .long exchange_byte
_RemoteDebugSize:                       .long endofcode-exchange_byte
_remoteex_general_illegal_instruction:  .long remoteex_general_illegal_instruction-exchange_byte
_remoteex_slot_illegal_instruction:     .long remoteex_slot_illegal_instruction-exchange_byte
_remoteex_cpu_address_error:            .long remoteex_cpu_address_error-exchange_byte
_remoteex_dma_address_error:            .long remoteex_dma_address_error-exchange_byte
_RemoteUBCHandler:                      .long RemoteUBCHandler-exchange_byte

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

exchange_byte:
    mov.l  R1, @-R15
    mov.l  STATPORT, r1
    ! Keep reading the status port until there's data ready
statloop:
    mov.b  @R1, R0
    cmp/eq #1, R0
    bf     statloop
    ! Alright, there's finally data ready. Let's do a byte exchange
    mov.l  INPORT, R0 ! retrieve from inport
    mov.b  @R0, R0
    extu.b R0, R0
    mov.l  OUTPORT, R1 ! Write to outport
    mov.b  R4, @R1
    ! Ok, we're done, time to cleanup
    rts
    mov.l  @r15+, r1

.align 4
OUTPORT:              .long 0x22080001
STATPORT:             .long 0x22100001
INPORT:               .long 0x22180001

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

SendLong:
    sts.l  pr, @-R15
    mov.l  R1, @-R15
    mov    R4, R1
    ! Send highest byte
    shlr16 R4
    bsr    exchange_byte
    shlr8  R4
    ! Send second-highest byte
    mov    R1, R4
    bsr    exchange_byte
    shlr16 R4
    ! Send second-lowest byte
    mov    R1, R4
    bsr    exchange_byte
    shlr8 R4
    ! Send lowest byte
    bsr    exchange_byte
    mov    R1, R4
    ! Alright, we're done
    mov.l  @r15+, r1
    rts
    lds.l  @r15+, pr

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

save_registers:
    stc.l   SR, @-r15
    stc.l   GBR, @-r15
    stc.l   VBR, @-r15
    sts.l   MACH, @-r15
    sts.l   MACL, @-r15
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
    rts
    mov.l   r15, @-r15 ! this can't be right

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

send_exception_data:
    sts     PR, R8
    mov     R4, R0

    ! Send the data back to computer(use 0x01 return command)
    bsr     exchange_byte
    mov     #1, R4

    ! exception type
    bsr     exchange_byte
    mov     R0, R4

    ! Send all Registers to computer
    mov     #21, r1
genregloop:
    bsr     SendLong
    mov.l   @r15+, r4
    cmp/pl  r1
    bf/s    genregloop
    add     #-1, R1
    lds     R8, PR
    rts
    nop

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

remoteex_general_illegal_instruction:
    ! Save all registers to stack
    sts.l   PR, @-r15
    bsr     save_registers
    nop

    ! Now that we've insured that we have the previous program's state at crash
    ! Let's start doing our own thing

    ! Send the initial exception data
    bsr     send_exception_data
    mov     #0, R4

    ! Wait in a loop until the computer tells us what to do
giloop:
    bra giloop
    nop

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

remoteex_slot_illegal_instruction:
    ! Save all registers to stack
    sts.l   PR, @-r15
    bsr     save_registers
    nop

    ! Now that we've insured that we have the previous program's state at crash
    ! Let's start doing our own thing

    ! Send the initial exception data
    bsr     send_exception_data
    mov     #1, R4

    ! Wait in a loop until the computer tells us what to do
siloop:
    bra siloop
    nop

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

remoteex_cpu_address_error:
    ! Save all registers to stack
    sts.l   PR, @-r15
    bsr     save_registers
    nop

    ! Now that we've insured that we have the previous program's state at crash
    ! Let's start doing our own thing

    ! Send the initial exception data
    bsr     send_exception_data
    mov     #1, R4

    ! Wait in a loop until the computer tells us what to do
caloop:
    bra caloop
    nop

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

remoteex_dma_address_error:
    ! Save all registers to stack
    sts.l   PR, @-r15
    bsr     save_registers
    nop

    ! Now that we've insured that we have the previous program's state at crash
    ! Let's start doing our own thing

    ! Send the initial exception data
    bsr     send_exception_data
    mov     #1, R4

    ! Wait in a loop until the computer tells us what to do
daloop:
    bra daloop
    nop

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

RemoteUBCHandler:
    ! Save all registers to stack
    sts.l   PR, @-r15
    bsr     save_registers
    nop

    ! Now that we've insured that we have the previous program's state at crash
    ! Let's start doing our own thing

ubcloop:
    bra ubcloop
    nop

.align 4

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

endofcode:
