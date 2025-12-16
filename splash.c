#include <6502.h>
#include <cx16.h>
#include <conio.h>
#include <string.h>

#include "splash.h"

#define     ADDR_VERSION      ((unsigned char*)0xff80)
#define     REVERSE_ON      0x12
#define     REVERSE_OFF     0x92

#define     CH_DOWN         17
#define     CH_LEFT         157
#define     CH_REVERSE_ON   18
#define     CH_REVERSE_OFF  146
#define     CH_FILL_ANGLE   169

char isProto()
{
    return ADDR_VERSION[0] > 0xf8? 'p' : ' ';
}

unsigned char getVersion()
{
    return isProto()? 0x100 - ADDR_VERSION[0] : ADDR_VERSION[0];
}

char petsciiLogo[] = {
    CH_LIGHTBLUE, CH_REVERSE_ON, CH_FILL_ANGLE, ' ', ' ', CH_LEFT, CH_LEFT, CH_LEFT, CH_DOWN,
    ' ', CH_REVERSE_OFF, ' ', ' ', CH_REVERSE_ON, ' ', CH_REVERSE_OFF, CH_FILL_ANGLE, CH_LEFT, CH_LEFT, CH_LEFT, CH_LEFT, CH_LEFT, CH_DOWN,
    CH_REVERSE_ON, ' ', CH_REVERSE_OFF, ' ', ' ', CH_RED, CH_REVERSE_ON, 163, 223, CH_LEFT, CH_LEFT, CH_LEFT, CH_LEFT, CH_LEFT, CH_DOWN,
    CH_LIGHTBLUE, REVERSE_OFF, 223, REVERSE_ON, ' ', ' ', REVERSE_OFF
};

void logo(unsigned char x, unsigned char y)
{
    int i;
    gotoxy(x,y);
    for(i=0; i<45; ++i)
          cbm_k_bsout(petsciiLogo[i]);
}

void login(char *name, char *date)
{
   unsigned char version = getVersion();
   char proto = isProto();

   bgcolor(COLOR_BLACK);
   clrscr();
   textcolor(COLOR_WHITE);
   cputsxy(40-strlen(name)/2,1,name);

   textcolor(COLOR_GREEN);
   gotoxy(2,1);
   cprintf("r%03d%c\r\n", version, proto);

   gotoy(5);
   cputs("   '##     '## '######## '##        '######   '#######  '##    '## '########   \r\n");
   cputs("    ## '##  ##  ##.....   ##       '##... ## '##.... ##  ###  '###  ##.....    \r\n");
   cputs("    ##  ##  ##  ##        ##        ##   ..   ##     ##  ####'####  ##         \r\n");
   cputs("    ##  ##  ##  ######    ##        ##        ##     ##  ## ### ##  ######     \r\n");
   cputs("    ##  ##  ##  ##...     ##        ##        ##     ##  ##. #  ##  ##...      \r\n");
   cputs("    ##  ##  ##  ##        ##        ##    ##  ##     ##  ## .   ##  ##         \r\n");
   cputs("   . ###. ###   ########  ######## . ######  . #######   ##     ##  ########   \r\n");
   cputs("    ...  ...   ........  ........   ......    .......   ..     ..  ........    \r\n");
   cputs("                                                                               \r\n\r\n");
   cputs( "********************************************************************************\r\n\r\n");
   cputs( "         not a warning!  all are welcome to access this system; there are no\r\n\r\n");
   cputs( "         penalties, nor any system security policy, and  no applicable state\r\n\r\n");
   cputs( "         or federal laws. sessions & e-mail are not monitored, as the system\r\n\r\n"); 
   cputs( "         has neither.     * * * note: system will be down if it is shut off.\r\n\r\n\r\n");
   cputs( "********************************************************************************\r\n");
   cprintf("                        commander x16 | %s", date);

   logo(2,19);
   gotoxy(0,31);
}
