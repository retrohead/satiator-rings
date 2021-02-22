// Almost everything is done here. I just need to finish the AR polling code
// which should use the UBC. Also the UBC handler needs to be implemented
// (in remoteex.s). I think I may remove the Local debugger altogether
// I may also consider adding some profiler code

#include "../iapetus.h"

#define BARA        (*(volatile u32 *)0xFFFFFF40)
#define BAMRA       (*(volatile u32 *)0xFFFFFF44)
#define BBRA        (*(volatile u16 *)0xFFFFFF48)
#define BARB        (*(volatile u32 *)0xFFFFFF60)
#define BAMRB       (*(volatile u32 *)0xFFFFFF64)
#define BBRB        (*(volatile u16 *)0xFFFFFF68)
#define BDRB        (*(volatile u32 *)0xFFFFFF70)
#define BDMRB       (*(volatile u32 *)0xFFFFFF74)
#define BRCR        (*(volatile u16 *)0xFFFFFF78)

void ex_general_illegal_instruction(void);
void ex_slot_illegal_instruction(void);
void ex_cpu_address_error(void);
void ex_dma_address_error(void);
void UBCHandler(void) __attribute__ ((interrupt_handler));

#if 0
//////////////////////////////////////////////////////////////////////////////

int RemoteDebuggerStart(void *addr)
{
   u32 *outbuf=(u32 *)addr;
   u32 *buf=(u32 *)RemoteDebugStart;
   u32 i;

   // Copy over code
   for (i = 0; i < RemoteDebugSize; i++)
      outbuf[i] = buf[i];

   // Setup exception functions
   bios_set_sh2_interrupt(0x4, addr+remoteex_general_illegal_instruction);
   bios_set_sh2_interrupt(0x6, addr+remoteex_slot_illegal_instruction);
   bios_set_sh2_interrupt(0x9, addr+remoteex_cpu_address_error);
   bios_set_sh2_interrupt(0xA, addr+remoteex_dma_address_error);
   bios_set_sh2_interrupt(0xC, addr+RemoteUBCHandler);

   // Hijack interrupt handling function here
//   if (so and so word == ?)
//      replace word with bra instruction

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int RemoteDebuggerStop(void)
{
   bios_set_sh2_interrupt(0x4, 0);
   bios_set_sh2_interrupt(0x6, 0);
   bios_set_sh2_interrupt(0x9, 0);
   bios_set_sh2_interrupt(0xA, 0);
   bios_set_sh2_interrupt(0xC, 0);

   // Give interrupt handling function control back here

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
#endif

void UBCHandler(void)
{
//   if (BRCR & ?)
}

//////////////////////////////////////////////////////////////////////////////

int debugger_start(void)
{
   // This function basically sets up exception handlers, disables remote
   // debugging, and sets things up to allows for breakpoints
   int ret;

   if ((ret = debugger_stop()) != IAPETUS_ERR_OK)
      return ret;

   bios_set_sh2_interrupt(0x4, ex_general_illegal_instruction);
   bios_set_sh2_interrupt(0x6, ex_slot_illegal_instruction);
   bios_set_sh2_interrupt(0x9, ex_cpu_address_error);
   bios_set_sh2_interrupt(0xA, ex_dma_address_error);

   // Clear the UBC registers
   BBRA = 0;
   BBRB = 0;
   BDRB = 0;
   BRCR = 0;
   bios_set_sh2_interrupt(0xC, UBCHandler);

   // Adjust level mask
   if (interrupt_get_level_mask() > 0xE)
      interrupt_set_level_mask(0xE);

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void debugger_set_code_breakpoint(u32 addr)
{
   BARA = addr;
   BAMRA = 0;

   // break on cpu cycles, instruction fetch cycles, read/write cycles
   BBRA = (0x1 << 6) | (0x1 << 4) | (0x3 << 2) | 0; 

   // Clear the condition match flags, break before instruction execution
   BRCR = (BRCR & 0x3BFF);
}

//////////////////////////////////////////////////////////////////////////////

void debugger_clear_code_breakpoint()
{
   BBRA = 0;
}

//////////////////////////////////////////////////////////////////////////////

void debugger_set_memory_breakpoint(u32 addr, u32 val, u32 valmask, int rw, int size)
{
   BARB = addr;
   BAMRB = 0;
   BDRB = val;
   BDMRB = valmask;

   // break on cpu cycles, data access cycles, etc. 
   BBRB = (0x1 << 6) | (0x2 << 4) | ((rw & 0x3) << 2) | (size & 0x3);
  
   // Clear the condition match flags, break before instruction execution
   BRCR = (BRCR & 0xFF33);
}

//////////////////////////////////////////////////////////////////////////////

void debugger_clear_memory_breakpoint(u32 addr)
{
   BBRB = 0;
}

//////////////////////////////////////////////////////////////////////////////

int debugger_stop(void)
{
   bios_set_sh2_interrupt(0x4, 0);
   bios_set_sh2_interrupt(0x6, 0);
   bios_set_sh2_interrupt(0x9, 0);
   bios_set_sh2_interrupt(0xA, 0);
   bios_set_sh2_interrupt(0xC, 0);

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
