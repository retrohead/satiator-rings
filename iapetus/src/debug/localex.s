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

! Things to finish:
! -Replace remaining old code to new

.macro save_registers
sts.l   PR, @-r15
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
mov     r15, r0
add     #0x58, r0
mov.l   r0, @-r15
.endm

! Global Data !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 4
debugdispsettings:
! bitmap specific
disp_bitmapsize:       .byte 0 ! BG_BITMAP512x256
! tile specific
disp_charsize:         .byte 0
disp_patternnamesize:  .byte 0
disp_planesize:        .byte 0
disp_map:              .byte 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
disp_flipfunction:     .byte 0
disp_extracharnum:     .byte 0
! used by both bitmap and tile
disp_transparentbit:   .byte 0
disp_color:            .byte 1 ! BG_256COLOR
disp_isbitmap:         .byte 1
disp_specialpriority:  .byte 0
disp_specialcolorcalc: .byte 0
disp_extrapalettenum:  .byte 0
disp_mapoffset:        .byte 0
! rotation specific
disp_rotationmode:     .byte 0
disp_padding:          .byte 0, 0
disp_parameteraddr:    .long 0x25E60000

.align 4
debug_font:
font_width:            .byte 8
font_height:           .byte 8
font_bpp:              .byte 1
font_padding:          .byte 1
font_data:             .long _font_8x8
font_charsize:         .long 0
font_lineinc:          .long 0
out:                   .long 0x25E00000
font_drawchar:         .long 0
font_screen:           .long 0
font_transparent:      .long 0

! ex_general_illegal_instruction !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.global _ex_general_illegal_instruction
_ex_general_illegal_instruction:
! Save all registers to stack
save_registers

mov.l   aGeneralInst_ptr, r10 ! message

! Let's display everything
mov.l   dispstuff_ptr,r1
jmp    @r1
nop

.align 4
dispstuff_ptr:    .long dispstuff
aGeneralInst_ptr: .long aGeneralInst
aGeneralInst:     .ascii "General Illegal Instruction"

! ex_slot_illegal_instruction !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.align 4
.global _ex_slot_illegal_instruction
_ex_slot_illegal_instruction:
! Save all registers to stack
save_registers

mov.l   aSlotInst_ptr, r10 ! message

! Let's display everything
mov.l   dispstuff_ptr_2,r1
jmp    @r1
nop

.align 4
dispstuff_ptr_2:    .long dispstuff
aSlotInst_ptr:      .long aSlotInst
aSlotInst:          .ascii "Slot Illegal Instruction"

! ex_cpu_address_error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.align 4
.global _ex_cpu_address_error
_ex_cpu_address_error:
! Save all registers to stack
save_registers

mov.l   aCPUAddr_ptr, r10 ! message

! Let's display everything
mov.l   dispstuff_ptr_3,r1
jmp    @r1
nop

.align 4
dispstuff_ptr_3:    .long dispstuff
aCPUAddr_ptr:       .long aCPUAddr
aCPUAddr:           .ascii "CPU Address Error"

! ex_dma_address_error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.align 4
.global _ex_dma_address_error
_ex_dma_address_error:
! Save all registers to stack
save_registers

mov.l   aDMAAddr_ptr, r10 ! message

mov.l   dispstuff_ptr_4,r1
jmp     @r1
nop

.align 4
dispstuff_ptr_4:    .long dispstuff
aDMAAddr_ptr:       .long aDMAAddr
aDMAAddr:           .ascii "DMA Address Error"

! dispstuff !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.align 4
dispstuff:
! Init Iapetus
mov.l   init_iapetus_ptr, r1
jsr     @r1
mov     #0, r4 ! RES_320x224

! Setup RBG0 Screen
mov.l   debugdispsettings_ptr, r4
mov.l   vdp_rbg0_init_ptr, r1
jsr     @r1
nop

! Use the default palette
mov.l   vdp_set_default_palette_ptr, r1
jsr     @r1
nop

! Setup an 8x8 1BPP font
mov     #0, r6 ! Non-transparent text
mov.l   debug_font_ptr, r5
mov.l   vdp_set_font_ptr, r1
jsr     @r1
mov     #4, r4 ! SCREEN_RBG0

! Render Error Text
! void vdp_print_text(font_struct *font, int x, int y, int color, const char *text)
add     #-4, r15
mov.l   r10, @r15 ! text
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0x8, r6 ! y
mov.l   vdp_print_text_ptr,r1
jsr     @r1
mov     #0x10, r5 ! x
add     #4, r15

! Render Registers R0-R15
mov     #0x10 r8
mov     #0x90, r9
extu.b  r9, r9
mov     r15, r11
regdisploop1:
add     #-0x1, r8

! Display Register Name and Value
add     #-0xC, r15

! For Reg 0-9 we want an extra space
mov     #0xA, r0
cmp/ge  r0, r8
bf      useregname2
mov.l   aRegName1_ptr, r0
bra     regname1_done
nop
useregname2:
mov.l   aRegName2_ptr, r0
regname1_done:

mov.l   r0, @r15   !aRegName1_ptr/aRegName2_ptr
mov.l   r8, @(4, r15) ! Register Number
mov.l   @r11+, r0
mov.l   r0, @(8, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     r9, r6 ! y
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0xC, r15

add     #-0x8, r9
cmp/pl  r8
bt      regdisploop1

! Render MACL Register
add     #-0x8, r15
mov.l   aMACLName_ptr, r0
mov.l   r0, @r15   ! aMACLName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xB8, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render MACH Register
add     #-0x8, r15
mov.l   aMACHName_ptr, r0
mov.l   r0, @r15   ! aMACHName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xB0, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render VBR Register
add     #-0x8, r15
mov.l   aVBRName_ptr, r0
mov.l   r0, @r15   ! aVBRName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xA8, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render GBR Register
add     #-0x8, r15
mov.l   aGBRName_ptr, r0
mov.l   r0, @r15   ! aGBRName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xA0, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render PR Register
add     #-0x8, r15
mov.l   aPRName_ptr, r0
mov.l   r0, @r15   ! aPRName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xC0, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render PC Register
add     #-0x8, r15
mov.l   aPCName_ptr, r0
mov.l   r0, @r15   ! aPCName_ptr
mov.l   @r11+, r0
add     #-2, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0xC8, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Render SR Register
add     #-0x8, r15
mov.l   aSRName_ptr, r0
mov.l   r0, @r15   ! aSRName_ptr
mov.l   @r11+, r0
mov.l   r0, @(4, r15) ! Register Value
mov.l   debug_font_ptr, r4
mov     #0xF, r7 ! color
mov     #0x98, r6 ! y
extu.b  r6,r6
mov.l   vdp_printf_ptr,r1
jsr     @r1
mov     #0, r5 ! x
add     #0x8, r15

! Display On
mov.l   vdp_disp_on_ptr,r1
jsr     @r1
nop

mov.l   commlink_start_service_ptr, r1
jsr     @r1
nop

endlessloop:
mov.l   vdp_vsync_ptr, r1
jsr     @r1
nop

mov.l   per_ptr, r1
add     #2, r1
mov.w   @r1, r1
extu.w  r1, r1
shlr8   r1
shlr2   r1
mov     r1, r0
and     #1, r0
extu.b  r0, r1
tst     r1, r1
bf      reset
bra     endlessloop
nop
reset:
mov.l   smpc_command_ptr, r1
jsr     @r1
mov     #0xD, r4 ! SMPC_CMD_SYSRES

colorf0_5:        .word 0x00F0
.align 4
init_iapetus_ptr:  .long _init_iapetus
debugdispsettings_ptr: .long debugdispsettings
vdp_rbg0_init_ptr:  .long _vdp_rbg0_init
vdp_set_default_palette_ptr: .long _vdp_set_default_palette
debug_font_ptr:    .long debug_font
vdp_set_font_ptr:   .long _vdp_set_font
vdp_printf_ptr:    .long _vdp_printf
vdp_print_text_ptr: .long _vdp_print_text
vdp_vsync_ptr:      .long _vdp_vsync
per_ptr:          .long _per
smpc_command_ptr:  .long _smpc_command
aRegName1_ptr:    .long aRegName1
aRegName2_ptr:    .long aRegName2
aMACLName_ptr:    .long aMACLName
aMACHName_ptr:    .long aMACHName
aVBRName_ptr:     .long aVBRName
aGBRName_ptr:     .long aGBRName
aPRName_ptr:      .long aPRName
aPCName_ptr:      .long aPCName
aSRName_ptr:      .long aSRName
aRegName1:        .ascii "R%d:  %08X"
.align 4
aRegName2:        .ascii "R%d:   %08X"
.align 4
aMACLName:        .ascii "MACL: %08X"
.align 4
aMACHName:        .ascii "MACH: %08X"
.align 4
aVBRName:         .ascii "VBR:  %08X"
.align 4
aGBRName:         .ascii "GBR:  %08X"
.align 4
aPRName:          .ascii "PR:   %08X"
.align 4
aPCName:          .ascii "PC:   %08X"
.align 4
aSRName:          .ascii "SR:   %08X"
.align 4
vdp_disp_on_ptr:    .long _vdp_disp_on
commlink_start_service_ptr: .long _commlink_start_service

