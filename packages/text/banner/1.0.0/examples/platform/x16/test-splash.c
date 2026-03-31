//
// cl65 --cpu 65c02 -t cx16 -Ors -o TEST-SPLASH.PRG test-splash.c splash.c 
//
#include <6502.h>
#include <cbm.h>
#include <stdio.h>

#include "splash.h"

// void setPETFont()
// {
//    // set PET font
//    struct regs fontregs;
//    fontregs.a = 4; // PET-like
//    fontregs.pc = 0xff62;
//    _sys(&fontregs);
// }

void main()
{
   //setPETFont();  
   cbm_k_bsout(CH_FONT_UPPER); 
   splashBanner("hello world");
   puts("\n\n\n\n");
}
