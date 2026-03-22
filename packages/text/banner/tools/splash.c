
#include <peekpoke.h>
#include <6502.h>
#include <cbm.h>

#define BANNER_LOAD_ADDR        0x02
#define BANNER_EXEC             0xA000
#define RAM_BANK_REG            0x00
#define BANNER_RAM_BANK         1

void splashBanner(char* string)
{
   struct regs r;
   r.a = 0;
   r.x = 0;
   r.y = 0;
   r.flags = 0;
   r.pc = BANNER_EXEC;

   POKE(RAM_BANK_REG, BANNER_RAM_BANK); // cc65 crt0 resets to bank 0; font lives in bank 1

   while(*string)
   {
      POKE(BANNER_LOAD_ADDR, *string);
      _sys(&r);
      ++string;
   }
}
