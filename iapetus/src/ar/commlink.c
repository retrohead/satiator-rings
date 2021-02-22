/*  Copyright 2006,2013 Theo Berkau

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

#include "../iapetus.h"

volatile int commlink_service;
void (*commlink_func)(u8);

//////////////////////////////////////////////////////////////////////////////

#define PAR_OUTPORT *((volatile u8 *)0x22080001)
#define PAR_STATPORT *((volatile u8 *)0x22100001)
#define PAR_INPORT *((volatile u8 *)0x22180001)

//////////////////////////////////////////////////////////////////////////////

u8 cl_exchange_byte(u8 val)
{
   u8 ret;
   while (!(PAR_STATPORT & 0x1)) {}
   ret = PAR_INPORT;
   PAR_OUTPORT = val;
	PAR_STATPORT = 0;
  return ret;
}

//////////////////////////////////////////////////////////////////////////////

u8 cl_receive_byte(void)
{
   u8 ret=0;
   while (!(PAR_STATPORT & 0x1)) {}
   ret = PAR_INPORT;
   PAR_OUTPORT = ret;
	PAR_STATPORT = 0;
   return ret;
}

//////////////////////////////////////////////////////////////////////////////

void cl_send_long(u32 val)
{
   cl_exchange_byte(val >> 24);
   cl_exchange_byte(val >> 16);
   cl_exchange_byte(val >> 8);
   cl_exchange_byte(val & 0xFF);
}

//////////////////////////////////////////////////////////////////////////////

u32 cl_receive_long(void)
{
   u32 ret;
   ret = cl_exchange_byte(0x00) << 24;
   ret |= cl_exchange_byte(0x00) << 16;
   ret |= cl_exchange_byte(0x00) << 8;
   ret |= cl_exchange_byte(0x00);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////

void cl_check1(u8 unused)
{
   u8 buf[2];
   s32 addr;
   s32 size;
   u16 chksum;
   u16 calcchksum;
   u8 data;

   // Clear outport
   PAR_OUTPORT = 0;

   // Check to see if someone's trying to send something
   if (PAR_INPORT == 'W')
   {
      for (;;)
      {
         buf[0] = cl_exchange_byte('R');
         buf[1] = cl_exchange_byte('W');

         if (buf[0] == 'W' && buf[1] == 'B')
            break;
      }

      // First byte seems to be unused
      cl_receive_byte();

      addr = cl_receive_byte() << 24;
      addr |= cl_receive_byte() << 16;
      addr |= cl_receive_byte() << 8;
      addr |= cl_receive_byte();

      size = cl_receive_byte() << 24;
      size |= cl_receive_byte() << 16;
      size |= cl_receive_byte() << 8;
      size |= cl_receive_byte();

      calcchksum = 0;

      // Receive data
      while (size > 0)
      {
         data = cl_receive_byte();
         *((volatile u8 *)addr) = data;
         addr++;
         calcchksum += data;
         calcchksum &= 0xFFF;
         size--;
      }

      chksum = cl_receive_byte() << 8;
      chksum |= cl_receive_byte();

      if (chksum == calcchksum)
      {
         PAR_OUTPORT = 0;
         ((void (*)())addr)();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

void cl_check2(u8 val)
{
   u32 addr;
   u32 size;
   u8 exec;
   u8 data=val;

   if (PAR_INPORT != 'I')
      return;

   // Sync with commlink
   for (;;)
   {
      if (cl_exchange_byte('D') != 'I')
         continue;

      if (cl_exchange_byte('O') == 'N')
         break;
   }

   switch(cl_exchange_byte(0x00))
   {
      case 0x01:
      {
         u8 chksum;

         // Download Memory
         cl_send_long(val);

         for (;;)
         {
            data = 0;
            data = cl_exchange_byte(data);
            addr = data << 24;
            data = cl_exchange_byte(data);
            addr |= data << 16;
            data = cl_exchange_byte(data);
            addr |= data << 8;
            data = cl_exchange_byte(data);
            addr |= data;

            data = cl_exchange_byte(data);
            size = data << 24;
            data = cl_exchange_byte(data);
            size |= data << 16;
            data = cl_exchange_byte(data);
            size |= data << 8;
            data = cl_exchange_byte(data);
            size |= data;

            // Transfer data
            if (size == 0)
               break;

            chksum = 0;

            while (size > 0)
            {
               data = *((volatile u8 *)addr);
               chksum += data;
               cl_exchange_byte(data);
               addr++;
               size--;
            }

            // Send the checksum
            cl_exchange_byte(chksum);
         }

         cl_exchange_byte('0');
         cl_exchange_byte('K');

         break;
      }
      case 0x02:
         // Set some kind of flag(FIXME)
         break;
      case 0x03:
         // Receive 5 long's, and do stuff(FIXME)
         cl_receive_long();
         cl_receive_long();
         cl_receive_long();
         cl_receive_long();
         cl_receive_long();
         break;
      case 0x04:
         // Send 5 long's(FIXME), used by AR software's download/upload saves features
         cl_send_long(0);
         cl_send_long(0);
         cl_send_long(0);
         cl_send_long(0);
         cl_send_long(0);
         break;
      case 0x05:
         // Exchange byte, write received byte to 0x060FFE20(FIXME), used by AR software's download/upload saves features
         cl_exchange_byte(0x00);
         break;
      case 0x06:
         // Receive 2 long's, write the first one to 0x060FE000, the second one to 0x060FE004(fix me), used by AR software's download/upload saves features(FIXME)
         cl_receive_long();
         cl_receive_long();
         break;
      case 0x07:
         // Send whatever is in R9. Looks like some kind of conditional variable. In the main menu it's 0x1, in other instances, it's 0x2, used by AR software's download/upload saves features
         cl_send_long(val);
         break;
      case 0x08:
      {         
         // Write byte to memory
         addr = cl_receive_long();
         *((volatile u8 *)addr) = cl_exchange_byte(0x00);
         break;
      }
      case 0x09:
      {
         u32 execaddr;

         // Upload(and execute) to Memory
         execaddr = addr = cl_receive_long();
         size = cl_receive_long();
         exec = cl_exchange_byte(0x00);

         // Transfer data
         while (size > 0)
         {
            data = cl_exchange_byte(data);
            *((volatile u8 *)addr) = data;
            addr++;
            size--;
         }

         if (exec)
            ((void (*)())execaddr)();

         break;
      }
      default: break;
   }
}

//////////////////////////////////////////////////////////////////////////////

void commlink_start_service(void)
{
   commlink_service = TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void commlink_stop_service()
{
   commlink_service = FALSE;
}

//////////////////////////////////////////////////////////////////////////////

void cl_set_service_func(void (*func)(u8))
{
	commlink_func = func;
}

//////////////////////////////////////////////////////////////////////////////






