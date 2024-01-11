#include <stdint.h>
#include <unistd.h>
#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#define  LOAD_TO_MAIN_RAM(addr)   cbm_k_load(0,addr)
#define  LOAD_TO_VERA(addr)       cbm_k_load(2,addr)
#define  STEREO_VOLUME(vol)       (vol + 196)
#define ABI(x) = ((unsigned char *)0x02[x]);

#define  ADSR_ENVELOPE_DATA   ((unsigned char*) 0x400)
#define  ADSR_INSTALLER       0x0460
#define  ADSR_ACTIVATE_VOICE  0x0463
#define  ADSR_VOICE7_TEST     0x0465
#define  ADSR_HANDLER_OFF     0x0468
#define  ADSR_HANDLER_ON      0x046a

void set_PET_font()
{
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void main()
{
   struct regs myregs;
   
   set_PET_font();

   cbm_k_setnam("adsr1.prg");
   cbm_k_setlfs(0,8,0);
   LOAD_TO_MAIN_RAM(0x0400);

   myregs.pc = ADSR_INSTALLER;
   _sys(&myregs);

//   cbm_k_setnam("adsr1test.prg");
//   cbm_k_setlfs(0,8,0);
//   LOAD_TO_MAIN_RAM(0x0600);

   VERA.control      = 0;              // port 0
   VERA.address      = 0xf9c0;         // voice 0
   VERA.address_hi   = VERA_INC_1 + 1; // from cx16.h
   VERA.data0        = 2000 & 0xff;
   VERA.data0        = 2000 >> 8;
   VERA.data0        = STEREO_VOLUME(20);
   VERA.data0        = (3 << 6);

   myregs.x = 0;
   myregs.a = 20;
   myregs.pc = ADSR_ACTIVATE_VOICE;
   _sys(&myregs);

   sleep(5);

   myregs.pc = ADSR_HANDLER_ON;
   _sys(&myregs);

}

