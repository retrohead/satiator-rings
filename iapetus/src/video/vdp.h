/*  Copyright 2005-2007,2013 Theo Berkau

    This file is part of Iapetus.

    Iapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Iapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Iapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef VDP_H
#define VDP_H

// VDP1
#define VDP1_RAM	0x25C00000

#define VDP1_REG_TVMR	(*(volatile u16 *)0x25D00000)
#define VDP1_REG_FBCR	(*(volatile u16 *)0x25D00002)
#define VDP1_REG_PTMR	(*(volatile u16 *)0x25D00004)
#define VDP1_REG_EWDR	(*(volatile u16 *)0x25D00006)
#define VDP1_REG_EWLR	(*(volatile u16 *)0x25D00008)
#define VDP1_REG_EWRR	(*(volatile u16 *)0x25D0000A)
#define VDP1_REG_ENDR	(*(volatile u16 *)0x25D0000C)
#define VDP1_REG_EDSR	(*(volatile u16 *)0x25D00010)
#define VDP1_REG_LOPR	(*(volatile u16 *)0x25D00012)
#define VDP1_REG_COPR	(*(volatile u16 *)0x25D00014)
#define VDP1_REG_MODR	(*(volatile u16 *)0x25D00016)


enum VDP1_DRAW_MODE
{
      V1DM_AUTO, V1DM_MANUALERASE, V1DM_MANUALCHANGE, V1DM_MANUALERASECHANGE
};

enum VDP1_SCREEN_MODE
{
   V1SM_NORMAL=0,
   V1SM_HIGHRES=1,
   V1SM_ROTATE16=2,
   V1SM_ROTATE8=3,
   V1SM_HDTV=4,
};

enum VDP1_INT_MODE
{
   V1IM_NONINT=0,
   V1IM_INTEVEN=0x8,
   V1IM_INTODD=0xC,
};

// VDP2
#define VDP2_RAM						0x25E00000
#define VDP2_CRAM						0x25F00000

#define VDP2_REG_TVMD           (*(volatile u16 *)0x25F80000)
#define VDP2_REG_EXTEN          (*(volatile u16 *)0x25F80002)
#define VDP2_REG_TVSTAT         (*(volatile u16 *)0x25F80004)
#define VDP2_REG_VRSIZE         (*(volatile u16 *)0x25F80006)
#define VDP2_REG_HCNT           (*(volatile u16 *)0x25F80008)
#define VDP2_REG_VCNT           (*(volatile u16 *)0x25F8000A)
#define VDP2_REG_RAMCTL         (*(volatile u16 *)0x25F8000E)
#define VDP2_REG_CYCA0L         (*(volatile u16 *)0x25F80010)
#define VDP2_REG_CYCA0U         (*(volatile u16 *)0x25F80012)
#define VDP2_REG_CYCA1L         (*(volatile u16 *)0x25F80014)
#define VDP2_REG_CYCA1U         (*(volatile u16 *)0x25F80016)
#define VDP2_REG_CYCB0L         (*(volatile u16 *)0x25F80018)
#define VDP2_REG_CYCB0U         (*(volatile u16 *)0x25F8001A)
#define VDP2_REG_CYCB1L         (*(volatile u16 *)0x25F8001C)
#define VDP2_REG_CYCB1U         (*(volatile u16 *)0x25F8001E)
#define VDP2_REG_BGON           (*(volatile u16 *)0x25F80020)
#define VDP2_REG_MZCTL          (*(volatile u16 *)0x25F80022)
#define VDP2_REG_SFSEL          (*(volatile u16 *)0x25F80024)
#define VDP2_REG_SFCODE         (*(volatile u16 *)0x25F80026)
#define VDP2_REG_CHCTLA         (*(volatile u16 *)0x25F80028)
#define VDP2_REG_CHCTLB         (*(volatile u16 *)0x25F8002A)
#define VDP2_REG_BMPNA          (*(volatile u16 *)0x25F8002C)
#define VDP2_REG_BMPNB          (*(volatile u16 *)0x25F8002E)
#define VDP2_REG_PNCN0          (*(volatile u16 *)0x25F80030)
#define VDP2_REG_PNCN1          (*(volatile u16 *)0x25F80032)
#define VDP2_REG_PNCN2          (*(volatile u16 *)0x25F80034)
#define VDP2_REG_PNCN3          (*(volatile u16 *)0x25F80036)
#define VDP2_REG_PNCR           (*(volatile u16 *)0x25F80038)
#define VDP2_REG_PLSZ           (*(volatile u16 *)0x25F8003A)
#define VDP2_REG_MPOFN          (*(volatile u16 *)0x25F8003C)
#define VDP2_REG_MPOFR          (*(volatile u16 *)0x25F8003E)
#define VDP2_REG_MPABN0         (*(volatile u16 *)0x25F80040)
#define VDP2_REG_MPCDN0         (*(volatile u16 *)0x25F80042)
#define VDP2_REG_MPABN1         (*(volatile u16 *)0x25F80044)
#define VDP2_REG_MPCDN1         (*(volatile u16 *)0x25F80046)
#define VDP2_REG_MPABN2         (*(volatile u16 *)0x25F80048)
#define VDP2_REG_MPCDN2         (*(volatile u16 *)0x25F8004A)
#define VDP2_REG_MPABN3         (*(volatile u16 *)0x25F8004C)
#define VDP2_REG_MPCDN3         (*(volatile u16 *)0x25F8004E)
#define VDP2_REG_MPABRA         (*(volatile u16 *)0x25F80050)
#define VDP2_REG_MPCDRA         (*(volatile u16 *)0x25F80052)
#define VDP2_REG_MPEFRA         (*(volatile u16 *)0x25F80054)
#define VDP2_REG_MPGHRA         (*(volatile u16 *)0x25F80056)
#define VDP2_REG_MPIJRA         (*(volatile u16 *)0x25F80058)
#define VDP2_REG_MPKLRA         (*(volatile u16 *)0x25F8005A)
#define VDP2_REG_MPMNRA         (*(volatile u16 *)0x25F8005C)
#define VDP2_REG_MPOPRA         (*(volatile u16 *)0x25F8005E)
#define VDP2_REG_MPABRB         (*(volatile u16 *)0x25F80060)
#define VDP2_REG_MPCDRB         (*(volatile u16 *)0x25F80062)
#define VDP2_REG_MPEFRB         (*(volatile u16 *)0x25F80064)
#define VDP2_REG_MPGHRB         (*(volatile u16 *)0x25F80066)
#define VDP2_REG_MPIJRB         (*(volatile u16 *)0x25F80068)
#define VDP2_REG_MPKLRB         (*(volatile u16 *)0x25F8006A)
#define VDP2_REG_MPMNRB         (*(volatile u16 *)0x25F8006C)
#define VDP2_REG_MPOPRB         (*(volatile u16 *)0x25F8006E)
#define VDP2_REG_SCXIN0         (*(volatile u16 *)0x25F80070)
#define VDP2_REG_SCXDN0         (*(volatile u16 *)0x25F80072)
#define VDP2_REG_SCYIN0         (*(volatile u16 *)0x25F80074)
#define VDP2_REG_SCYDN0         (*(volatile u16 *)0x25F80076)
#define VDP2_REG_ZMXIN0         (*(volatile u16 *)0x25F80078)
#define VDP2_REG_ZMXDN0         (*(volatile u16 *)0x25F8007A)
#define VDP2_REG_ZMYIN0         (*(volatile u16 *)0x25F8007C)
#define VDP2_REG_ZMYDN0         (*(volatile u16 *)0x25F8007E)
#define VDP2_REG_SCXIN1         (*(volatile u16 *)0x25F80080)
#define VDP2_REG_SCXDN1         (*(volatile u16 *)0x25F80082)
#define VDP2_REG_SCYIN1         (*(volatile u16 *)0x25F80084)
#define VDP2_REG_SCYDN1         (*(volatile u16 *)0x25F80086)
#define VDP2_REG_ZMXIN1         (*(volatile u16 *)0x25F80088)
#define VDP2_REG_ZMXDN1         (*(volatile u16 *)0x25F8008A)
#define VDP2_REG_ZMYIN1         (*(volatile u16 *)0x25F8008C)
#define VDP2_REG_ZMYDN1         (*(volatile u16 *)0x25F8008E)
#define VDP2_REG_SCXN2          (*(volatile u16 *)0x25F80090)
#define VDP2_REG_SCYN2          (*(volatile u16 *)0x25F80092)
#define VDP2_REG_SCXN3          (*(volatile u16 *)0x25F80094)
#define VDP2_REG_SCYN3          (*(volatile u16 *)0x25F80096)
#define VDP2_REG_ZMCTL          (*(volatile u16 *)0x25F80098)
#define VDP2_REG_SCRCTL         (*(volatile u16 *)0x25F8009A)
#define VDP2_REG_VCSTAU         (*(volatile u16 *)0x25F8009C)
#define VDP2_REG_VCSTAL         (*(volatile u16 *)0x25F8009E)
#define VDP2_REG_LSTA0U         (*(volatile u16 *)0x25F800A0)
#define VDP2_REG_LSTA0L         (*(volatile u16 *)0x25F800A2)
#define VDP2_REG_LSTA1U         (*(volatile u16 *)0x25F800A4)
#define VDP2_REG_LSTA1L         (*(volatile u16 *)0x25F800A6)
#define VDP2_REG_LCTAU          (*(volatile u16 *)0x25F800A8)
#define VDP2_REG_LCTAL          (*(volatile u16 *)0x25F800AA)
#define VDP2_REG_BKTA           (*(volatile u32 *)0x25F800AC)
#define VDP2_REG_RPMD           (*(volatile u16 *)0x25F800B0)
#define VDP2_REG_RPRCTL         (*(volatile u16 *)0x25F800B2)
#define VDP2_REG_KTCTL          (*(volatile u16 *)0x25F800B4)
#define VDP2_REG_KTAOF          (*(volatile u16 *)0x25F800B6)
#define VDP2_REG_OVPNRA         (*(volatile u16 *)0x25F800B8)
#define VDP2_REG_OVPNRB         (*(volatile u16 *)0x25F800BA)
#define VDP2_REG_RPTAU          (*(volatile u16 *)0x25F800BC)
#define VDP2_REG_RPTAL          (*(volatile u16 *)0x25F800BE)
#define VDP2_REG_WPSX0          (*(volatile u16 *)0x25F800C0)
#define VDP2_REG_WPSY0          (*(volatile u16 *)0x25F800C2)
#define VDP2_REG_WPEX0          (*(volatile u16 *)0x25F800C4)
#define VDP2_REG_WPEY0          (*(volatile u16 *)0x25F800C6)
#define VDP2_REG_WPSX1          (*(volatile u16 *)0x25F800C8)
#define VDP2_REG_WPSY1          (*(volatile u16 *)0x25F800CA)
#define VDP2_REG_WPEX1          (*(volatile u16 *)0x25F800CC)
#define VDP2_REG_WPEY1          (*(volatile u16 *)0x25F800CE)
#define VDP2_REG_WCTLA          (*(volatile u16 *)0x25F800D0)
#define VDP2_REG_WCTLB          (*(volatile u16 *)0x25F800D2)
#define VDP2_REG_WCTLC          (*(volatile u16 *)0x25F800D4)
#define VDP2_REG_WCTLD          (*(volatile u16 *)0x25F800D6)
#define VDP2_REG_LWTA0U         (*(volatile u16 *)0x25F800D8)
#define VDP2_REG_LWTA0L         (*(volatile u16 *)0x25F800DA)
#define VDP2_REG_LWTA1U         (*(volatile u16 *)0x25F800DC)
#define VDP2_REG_LWTA1L         (*(volatile u16 *)0x25F800DE)
#define VDP2_REG_SPCTL          (*(volatile u16 *)0x25F800E0)
#define VDP2_REG_SDCTL          (*(volatile u16 *)0x25F800E2)
#define VDP2_REG_CRAOFA         (*(volatile u16 *)0x25F800E4)
#define VDP2_REG_CRAOFB         (*(volatile u16 *)0x25F800E6)
#define VDP2_REG_LNCLEN         (*(volatile u16 *)0x25F800E8)
#define VDP2_REG_SFPRMD         (*(volatile u16 *)0x25F800EA)
#define VDP2_REG_CCCTL          (*(volatile u16 *)0x25F800EC)
#define VDP2_REG_SFCCMD         (*(volatile u16 *)0x25F800EE)
#define VDP2_REG_PRISA          (*(volatile u16 *)0x25F800F0)
#define VDP2_REG_PRISB          (*(volatile u16 *)0x25F800F2)
#define VDP2_REG_PRISC          (*(volatile u16 *)0x25F800F4)
#define VDP2_REG_PRISD          (*(volatile u16 *)0x25F800F6)
#define VDP2_REG_PRINA          (*(volatile u16 *)0x25F800F8)
#define VDP2_REG_PRINB          (*(volatile u16 *)0x25F800FA)
#define VDP2_REG_PRIR           (*(volatile u16 *)0x25F800FC)
#define VDP2_REG_RESERVE        (*(volatile u16 *)0x25F800FE)
#define VDP2_REG_CCRSA          (*(volatile u16 *)0x25F80100)
#define VDP2_REG_CCRSB          (*(volatile u16 *)0x25F80102)
#define VDP2_REG_CCRSC          (*(volatile u16 *)0x25F80104)
#define VDP2_REG_CCRSD          (*(volatile u16 *)0x25F80106)
#define VDP2_REG_CCRNA          (*(volatile u16 *)0x25F80108)
#define VDP2_REG_CCRNB          (*(volatile u16 *)0x25F8010A)
#define VDP2_REG_CCRR           (*(volatile u16 *)0x25F8010C)
#define VDP2_REG_CCRLB          (*(volatile u16 *)0x25F8010E)
#define VDP2_REG_CLOFEN         (*(volatile u16 *)0x25F80110)
#define VDP2_REG_CLOFSL         (*(volatile u16 *)0x25F80112)
#define VDP2_REG_COAR           (*(volatile u16 *)0x25F80114)
#define VDP2_REG_COAG           (*(volatile u16 *)0x25F80116)
#define VDP2_REG_COAB           (*(volatile u16 *)0x25F80118)
#define VDP2_REG_COBR           (*(volatile u16 *)0x25F8011A)
#define VDP2_REG_COBG           (*(volatile u16 *)0x25F8011C)
#define VDP2_REG_COBB           (*(volatile u16 *)0x25F8011E)

typedef struct
{
   union
   {
      struct 
      {
	 u16 unused1:3;
	 u16 rbg0_trans_enab:1;
	 u16 nbg3_trans_enab:1;
	 u16 nbg2_trans_enab:1;
	 u16 nbg1_trans_enab:1;
	 u16 nbg0_trans_enab:1;
	 u16 unused2:2;
	 u16 rbg1_enab:1;
	 u16 rbg0_enab:1;
	 u16 nbg3_enab:1;
	 u16 nbg2_enab:1;
	 u16 nbg1_enab:1;
	 u16 nbg0_enab:1;
      } part;
      u16 all;
   } BGON;
   u8 exbgenab;
   union
   {
      struct 
      {
	 u16 unused1:2;
	 u16 n1chcn:2;
	 u16 n1bmsz:2;
	 u16 n1bmenab:1;
	 u16 n1chsz:1;
	 u16 unused2:1;
	 u16 n0chcn:3;
	 u16 n0bmsz:2;
	 u16 n0bmenab:1;
	 u16 n0chsz:1;
      } part;
      u16 all;
   } CHCTLA;
   union
   {
      struct 
      {
         u16 unused1:1;
         u16 r0chcn:3;
	 u16 unused2:1;
         u16 r0bmsz:1;
         u16 r0bmenab:1;
         u16 r0chsz:1;
         u16 unused3:2;
         u16 n3chcn:1;
         u16 n3chsz:1;
         u16 unused4:2;
         u16 n2chcn:1;
         u16 n2chsz:1;
      } part;
      u16 all;
   } CHCTLB;
   union
   {
      struct 
      {
	 u16 unused1:2;
         u16 n1bmpr:1;
         u16 n1bmcc:1;
	 u16 unused2:1;
         u16 n1bmp:3;
         u16 unused3:2;
         u16 n0bmpr:1;
         u16 n0bmcc:1;
         u16 unused4:1;
         u16 n0bmp:3;
      } part;
      u16 all;
   } BMPNA;
   union
   {
      struct 
      {
         u16 unused1:10;
         u16 r0bmpr:1;
         u16 r0bmcc:1;
         u16 unused4:1;
         u16 r0bmp:3;
      } part;
      u16 all;
   } BMPNB;
   union
   {
      struct 
      {
         u16 n0pnb:1;
         u16 n0cnsm:1;
         u16 unused1:4;
         u16 n0spr:1;
         u16 n0scc:1;
         u16 n0splt:3;
         u16 n0scn:5;
      } part;
      u16 all;
   } PNCN0;
   union
   {
      struct 
      {
         u16 n1pnb:1;
         u16 n1cnsm:1;
         u16 unused1:4;
         u16 n1spr:1;
         u16 n1scc:1;
         u16 n1splt:3;
         u16 n1scn:5;
      } part;
      u16 all;
   } PNCN1;
   union
   {
      struct 
      {
         u16 n2pnb:1;
         u16 n2cnsm:1;
         u16 unused1:4;
         u16 n2spr:1;
         u16 n2scc:1;
         u16 n2splt:3;
         u16 n2scn:5;
      } part;
      u16 all;
   } PNCN2;
   union
   {
      struct 
      {
         u16 n3pnb:1;
         u16 n3cnsm:1;
         u16 unused1:4;
         u16 n3spr:1;
         u16 n3scc:1;
         u16 n3splt:3;
         u16 n3scn:5;
      } part;
      u16 all;
   } PNCN3;
   union
   {
      struct 
      {
         u16 r0pnb:1;
         u16 r0cnsm:1;
         u16 unused1:4;
         u16 r0spr:1;
         u16 r0scc:1;
         u16 r0splt:3;
         u16 r0scn:5;
      } part;
      u16 all;
   } PNCR;
   union
   {
      struct 
      {
         u16 rbovr:2;
         u16 rbplsz:2;
         u16 raovr:2;
         u16 raplsz:2;
         u16 n3plsz:2;
         u16 n2plsz:2;
         u16 n1plsz:2;
         u16 n0plsz:2;
      } part;
      u16 all;
   } PLSZ;
   union
   {
      struct 
      {
         u16 unused1:1;
         u16 n3mp:3;
         u16 unused2:1;
         u16 n2mp:3;
         u16 unused3:1;
         u16 n1mp:3;
         u16 unused4:1;
         u16 n0mp:3;
      } part;
      u16 all;
   } MPOFN;
   union
   {
      struct 
      {
         u16 unused1:9;
         u16 rbmp:3;
         u16 unused4:1;
         u16 ramp:3;
      } part;
      u16 all;
   } MPOFR;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n0mpb:6;
         u16 unused2:2;
         u16 n0mpa:6;
      } part;
      u16 all;
   } MPABN0;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n0mpd:6;
         u16 unused2:2;
         u16 n0mpc:6;
      } part;
      u16 all;
   } MPCDN0;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n1mpb:6;
         u16 unused2:2;
         u16 n1mpa:6;
      } part;
      u16 all;
   } MPABN1;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n1mpd:6;
         u16 unused2:2;
         u16 n1mpc:6;
      } part;
      u16 all;
   } MPCDN1;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n2mpb:6;
         u16 unused2:2;
         u16 n2mpa:6;
      } part;
      u16 all;
   } MPABN2;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n2mpd:6;
         u16 unused2:2;
         u16 n2mpc:6;
      } part;
      u16 all;
   } MPCDN2;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n3mpb:6;
         u16 unused2:2;
         u16 n3mpa:6;
      } part;
      u16 all;
   } MPABN3;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 n3mpd:6;
         u16 unused2:2;
         u16 n3mpc:6;
      } part;
      u16 all;
   } MPCDN3;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampb:6;
         u16 unused2:2;
         u16 rampa:6;
      } part;
      u16 all;
   } MPABRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampd:6;
         u16 unused2:2;
         u16 rampc:6;
      } part;
      u16 all;
   } MPCDRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampf:6;
         u16 unused2:2;
         u16 rampe:6;
      } part;
      u16 all;
   } MPEFRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 ramph:6;
         u16 unused2:2;
         u16 rampg:6;
      } part;
      u16 all;
   } MPGHRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampj:6;
         u16 unused2:2;
         u16 rampi:6;
      } part;
      u16 all;
   } MPIJRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampl:6;
         u16 unused2:2;
         u16 rampk:6;
      } part;
      u16 all;
   } MPKLRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampn:6;
         u16 unused2:2;
         u16 rampm:6;
      } part;
      u16 all;
   } MPMNRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rampp:6;
         u16 unused2:2;
         u16 rampo:6;
      } part;
      u16 all;
   } MPOPRA;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpb:6;
         u16 unused2:2;
         u16 rbmpa:6;
      } part;
      u16 all;
   } MPABRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpd:6;
         u16 unused2:2;
         u16 rbmpc:6;
      } part;
      u16 all;
   } MPCDRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpf:6;
         u16 unused2:2;
         u16 rbmpe:6;
      } part;
      u16 all;
   } MPEFRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmph:6;
         u16 unused2:2;
         u16 rbmpg:6;
      } part;
      u16 all;
   } MPGHRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpj:6;
         u16 unused2:2;
         u16 rbmpi:6;
      } part;
      u16 all;
   } MPIJRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpl:6;
         u16 unused2:2;
         u16 rbmpk:6;
      } part;
      u16 all;
   } MPKLRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpn:6;
         u16 unused2:2;
         u16 rbmpm:6;
      } part;
      u16 all;
   } MPMNRB;
   union
   {
      struct 
      {
         u16 unused1:2;
         u16 rbmpp:6;
         u16 unused2:2;
         u16 rbmpo:6;
      } part;
      u16 all;
   } MPOPRB;
	union
	{
		struct
		{
			u16 unused1:6;
			u16 n1zm:2;
			u16 unused2:6;
			u16 n0zm:2;
		} part;
		u16 all;
	} ZMCTL;
	union
	{
		struct
		{
			u16 unused1:2;
			u16 n1lss:2;
			u16 n1lzmx:1;
			u16 n1lzmy:1;
			u16 n1lscx:1;
			u16 n1vcsc:1;
			u16 unused2:2;
			u16 n0lss:2;
			u16 n0lzmx:1;
			u16 n0lzmy:1;
			u16 n0lscx:1;
			u16 n0vcsc:1;
		} part;
		u16 all;
	} SCRCTL;
	union
	{
		union
		{
			u32 unused1:13;
			u32 vcstau:3;
			u32 vcstal:15;
			u32 unused2:1;
		} part;
		u32 all;
	} VCSTA;
	
   u32 RPTA;
   union
   {
      struct 
      {
	 u16 unused1:5;
	 u16 nbg1_priority:3;
	 u16 unused2:5;
	 u16 nbg0_priority:3;
      } part;
      u16 all;
   } PRINA;
   union
   {
      struct 
      {
	 u16 unused1:5;
	 u16 nbg3_priority:3;
	 u16 unused2:5;
	 u16 nbg2_priority:3;
      } part;
      u16 all;
   } PRINB;
   union
   {
      struct 
      {
	 u16 unused1:13;
	 u16 rbg0_priority:3;
      } part;
      u16 all;
   } PRIR;
   u16 CLOFEN;
   u16 CLOFSL;
   u32 pattern_name_addr[4];
   u32 char_pattern_addr[4];
   int pattern_name_cycles[4];
   int char_pattern_cycles[4];
   int screen_width;
   int screen_height;
} vdp2_settings_struct;

#define ZP_NONE         0x0
#define ZP_UPPERLEFT    0x5
#define ZP_UPPERCENTER  0x6
#define ZP_UPPERRIGHT   0x7
#define ZP_CENTERLEFT   0x9
#define ZP_CENTERCENTER 0xA
#define ZP_CENTERRIGHT  0xB
#define ZP_LOWERLEFT    0xD
#define ZP_LOWERCENTER  0xE
#define ZP_LOWERRIGHT   0xF

#define VDP_ERR_OK		0
#define VDP_ERR_CONFLICT	-1
#define VDP_ERR_INVALID		-2
#define VDP_ERR_PALONLY		-3

enum RESNUM
{
   RES_320x224 = 0x00,
   RES_352x224 = 0x01,
   RES_640x224 = 0x02,
   RES_704x224 = 0x03,
   RES_320x240 = 0x10,
   RES_352x240 = 0x11,
   RES_640x240 = 0x12,
   RES_704x240 = 0x13,
   RES_320x256 = 0x20,
   RES_352x256 = 0x21,
   RES_640x256 = 0x22,
   RES_704x256 = 0x23,
   RES_320x448 = 0xC0,
   RES_352x448 = 0xC1,
   RES_640x448 = 0xC2,
   RES_704x448 = 0xC3,
   RES_320x480 = 0xD0,
   RES_352x480 = 0xD1,
   RES_640x480 = 0xD2,
   RES_704x480 = 0xD3,
   RES_320x512 = 0xE0,
   RES_352x512 = 0xE1,
   RES_640x512 = 0xE2,
   RES_704x512 = 0xE3
};

typedef struct
{
   // bitmap specific
   u8 bitmap_size;
   // tile specific
   u8 char_size;
   u8 pattern_name_size;
   u8 plane_size;
   u8 map[16];
   u8 flip_function;
   u8 extra_char_num;
   // used by both bitmap and tile 
   u8 transparent_bit;
   u8 color;
   u8 is_bitmap;
   u8 special_priority;
   u8 special_color_calc;
   u8 extra_palette_num;
   u8 map_offset;
   // rotation specific
   u8 rotation_mode;
   u32 parameter_addr;
} screen_settings_struct;

enum SCREENNUM
{
   SCREEN_NBG0 = 0,
   SCREEN_NBG1 = 1,
   SCREEN_NBG2 = 2,
   SCREEN_NBG3 = 3,
   SCREEN_RBG0 = 4,
   SCREEN_RBG1 = 5,
   SCREEN_EXBG = 6,
   SCREEN_VDP1 = 7
};

enum BGSIZE
{
   BG_BITMAP512x256 = 0x00,
   BG_BITMAP512x512 = 0x01,
   BG_BITMAP1024x256 = 0x02,
   BG_BITMAP1024x512 = 0x03
};

enum BGCELLSIZE
{
   BG_CELL1x1 = 0x00,
   BG_CELL2x2 = 0x01
};

enum BGCOLOR
{
   BG_16COLOR = 0x0,
   BG_256COLOR = 0x1,
   BG_2048COLOR = 0x2,
   BG_32786COLOR = 0x3,
   BG_16MCOLOR = 0x4
};

enum CRMMODE
{
   CRM5_1024 = 0x0,
   CRM5_2048 = 0x1,
   CRM8_1024 = 0x2
};

typedef struct
{
   u32 Xst, Yst, Zst;
   u32 deltaXst, deltaYst;
   u32 deltaX, deltaY;
   u32 A, B, C, D, E, F;
   u16 Px, Py, Pz;
   u16 unused1;
   u16 Cx, Cy, Cz;
   u16 unused2;
   u32 Mx, My;
   u32 kx, ky;
} rottbl_struct;

#define SPRITE_JUMPNEXT                 0x00000000
#define SPRITE_JUMPASSIGN               0x01000000
#define SPRITE_JUMPCALL                 0x02000000
#define SPRITE_JUMPRETURN               0x03000000
#define SPRITE_SKIPNEXT                 0x04000000
#define SPRITE_SKIPASSIGN               0x05000000
#define SPRITE_SKIPCALL                 0x06000000
#define SPRITE_SKIPRETURN               0x07000000

#define SPRITE_ZOOMPOINTNONE            0x00000000
#define SPRITE_ZOOMPOINTUPPERLEFT       0x00500000
#define SPRITE_ZOOMPOINTUPPERCENTER     0x00600000
#define SPRITE_ZOOMPOINTUPPERRIGHT      0x00700000
#define SPRITE_ZOOMPOINTCENTERLEFT      0x00900000
#define SPRITE_ZOOMPOINTCENTERCENTER    0x00A00000
#define SPRITE_ZOOMPOINTCENTERRIGHT     0x00B00000
#define SPRITE_ZOOMPOINTLOWERLEFT       0x00D00000
#define SPRITE_ZOOMPOINTLOWERCENTER     0x00E00000
#define SPRITE_ZOOMPOINTLOWERRIGHT      0x00F00000

#define SPRITE_REVERSEHORIZONTAL        0x00010000
#define SPRITE_REVERSEVERTICAL          0x00020000

#define SPRITE_MSBOFF                   0x00000000
#define SPRITE_MSBON                    0x00008000

#define SPRITE_HIGHSPEEDSHRINKDISABLE   0x00000000
#define SPRITE_HIGHSPEEDSHRINKENABLE    0x00001000

#define SPRITE_PRECLIPDISABLE           0x00000800
#define SPRITE_PRECLIPENABLE            0x00000000

#define SPRITE_CLIPDISABLE              0x00000000
#define SPRITE_CLIPOUTSIDE              0x00000400
#define SPRITE_CLIPINSIDE               0x00000600

#define SPRITE_MESHDISABLE              0x00000000
#define SPRITE_MESHENABLE               0x00000100

#define SPRITE_ENDCODEDISABLE           0x00000080
#define SPRITE_ENDCODEENABLE            0x00000000

#define SPRITE_TRANSPIXELDISABLE        0x00000040
#define SPRITE_TRANSPIXELENABLE         0x00000000

#define SPRITE_4BPP                     0x00000000
#define SPRITE_4BPPLUT                  0x00000008
#define SPRITE_8BPP64COLOR              0x00000010
#define SPRITE_8BPP128COLOR             0x00000018
#define SPRITE_8BPP256COLOR             0x00000020
#define SPRITE_16BPP                    0x00000028

#define RGB16(r, g, b) (0x8000 | (((b) & 0x1F) << 10) | (((g) & 0x1F) << 5) | ((r) & 0x1F))

typedef struct
{
   u16 cmd_ctrl;
   u16 cmd_link;
   union
   {
      struct
      {
         u16 mon:1;
         u16 reserved:2;
         u16 hss:1;
         u16 pclp:1;
         u16 clip:1;
         u16 cmod:1;
         u16 mesh:1;
         u16 ecd:1;
         u16 spd:1;
         u16 color_mode:3;
         u16 color_calc:3;
      } part;
      u16 all;
   } cmd_pmod;
   u16 cmd_colr;
   u16 cmd_srca;
   u16 cmd_size;
   u16 cmd_xa;
   u16 cmd_ya;
   u16 cmd_xb;
   u16 cmd_yb;
   u16 cmd_xc;
   u16 cmd_yc;
   u16 cmd_xd;
   u16 cmd_yd;
   u16 cmd_grda;
   u16 cmd_unused;
} vdp1cmd_struct;

typedef struct
{
   u32 link_addr;
   u32 attr;
   u32 addr;
   u32 gouraud_addr;
   u16 bank;
   u16 width, height;
   s16 x;
   s16 y;
   s16 x2;
   s16 y2;
   s16 x3;
   s16 y3;
   s16 x4;
   s16 y4;
} sprite_struct;

extern volatile u16 *vdp1_ram;
extern volatile u16 *vdp2_ram;
extern volatile u16 *vdp2_cram;

void vdp_init(int res);
void vdp_set_priority(int screen, u8 priority);
int vdp_nbg0_init(screen_settings_struct *settings);
void vdp_nbg0_deinit(void);
int vdp_nbg1_init(screen_settings_struct *settings);
void vdp_nbg1_deinit(void);
int vdp_nbg2_init(screen_settings_struct *settings);
void vdp_nbg2_deinit(void);
int vdp_nbg3_init(screen_settings_struct *settings);
void vdp_nbg3_deinit(void);
int vdp_rbg0_init(screen_settings_struct *settings);
void vdp_rbg0_deinit(void);
int vdp_rbg1_init(screen_settings_struct *settings);
void vdp_rbg1_deinit(void);
int vdp_exbg_init(screen_settings_struct *settings);
void vdp_exbg_deinit(void);
void vdp_wait_hblankin(void);
void vdp_wait_hblankout(void);
void vdp_wait_vblankin(void);
void vdp_wait_vblankout(void);
void vdp_vsync(void);
void vdp_disp_on(void);
void vdp_disp_off(void);
void vdp_set_color_offset(u8 num, s16 r, s16 g, s16 b);
void vdp_enable_color_offset(u16 screen, int select);
void vdp_disable_color_offset(u16 screen);
void vdp_get_scr_width_height(int *width, int *height);

// Window functions
void vdp_enable_line_window(int screennum, int windownum, u16 mode, u32 linetbladdr);
void vdp_disable_line_window(void);

// Rotation Screen specific
void vdp_set_rotation_table(screen_settings_struct *settings, int num, rottbl_struct *tbl);

// Palette related
int vdp_set_palette(int type, void *palette, int size);
int vdp_set_default_palette(void);

// Sprite/Polygon related
void vdp1_set_draw_mode(enum VDP1_DRAW_MODE mode, enum VDP1_SCREEN_MODE tv_mode, enum VDP1_INT_MODE int_mode);
void vdp1_trigger_draw();
void vdp_system_clipping(sprite_struct *sprite);
void vdp_user_clipping(sprite_struct *sprite);
void vdp_local_coordinate(sprite_struct *sprite);
void vdp_draw_normal_sprite(sprite_struct *sprite);
void vdp_draw_scaled_sprite(sprite_struct *sprite);
void vdp_draw_distorted_sprite(sprite_struct *sprite);
void vdp_draw_polygon(sprite_struct *sprite);
void vdp_draw_poly_line(sprite_struct *sprite);
void vdp_draw_line(sprite_struct *sprite);
void vdp_start_draw_list();
void vdp_end_draw_list();

#endif
