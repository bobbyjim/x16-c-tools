#include <6502.h>
#include <cx16.h>

#include "font.h"

void setSystemFont(unsigned char font_id)
{
   // set PET font
   struct regs fontregs;
   fontregs.a = font_id;
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void setFontPET(void)
{
   setSystemFont(FONT_PET);
}
