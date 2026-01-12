#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <conio.h>
#include <6502.h>
#include <cbm.h>
#include <cx16.h>

#include "ADSR.h"

#define STEREO_VOLUME(vol)      (vol + 196)

// Waveforms
#define PSG_WAVE_SQUARE         63
#define PSG_WAVE_SAWTOOTH       (1 << 6)
#define PSG_WAVE_TRIANGLE       (2 << 6)
#define PSG_WAVE_NOISE          (3 << 6)

// Note frequencies
unsigned int tunedNotes[] = {
    0,    186,  197,  208,  221,  234,  248,  263,  278,  295,  312,  331,  // 0-11
    351,  372,  394,  417,  442,  468,  496,  526,  557,  590,  625,  662,  // 12-23
    702,  744,  788,  835,  884,  937,  993,  1052, 1114, 1181, 1251, 1325, // 24-35
    1404, 1488, 1576, 1670, 1769, 1875, 1986, 2104, 2229, 2362, 2502, 2651, // 36-47
    2809, 2976, 3153, 3340, 3539, 3750                                       // 48-53
};

// Test parameters
unsigned char wave_index = 2;  // 0=square, 1=sawtooth, 2=triangle, 3=noise
unsigned char note_index = 35; // F4
unsigned char attack   = 5;
unsigned char decay    = 30;
unsigned char sustain_level = 40;  // volume to decay to
unsigned char sustain_timer_fractional = 100; // fractional sustain duration
unsigned char release  = 50;

const char* waveforms[] = {"square", "sawtooth", "triangle", "noise"};
unsigned char wave_values[] = {PSG_WAVE_SQUARE, PSG_WAVE_SAWTOOTH, PSG_WAVE_TRIANGLE, PSG_WAVE_NOISE};

void set_PET_font()
{
   struct regs fontregs;
   fontregs.a = 4; // PET-like
   fontregs.pc = 0xff62;
   _sys(&fontregs);
}

void display_menu()
{
   gotoxy(0, 0);
   clrscr();
   
   cprintf("single note adsr tester\r\n");
   cprintf("=======================\r\n\r\n");
   
   cprintf("waveform: %s (w=cycle)\r\n", waveforms[wave_index]);
   cprintf("frequency: note %d (%u hz)  (h/n)\r\n", note_index, tunedNotes[note_index]);
   cprintf("\r\n");
   cprintf("attack:         %3d  (a/z)\r\n", attack);
   cprintf("decay:          %3d  (s/x)\r\n", decay);
   cprintf("sustain level:  %3d  (d/c)\r\n", sustain_level);
   cprintf("sustain frac:   %3d  (f/v)\r\n", sustain_timer_fractional);
   cprintf("release:        %3d  (g/b)\r\n", release);
   cprintf("\r\n");
   cprintf("space: trigger note\r\n");
   cprintf("q: quit\r\n");
}

void trigger_note()
{
   // Set frequency
   adsr_setFrequency(0, tunedNotes[note_index]);
   
   // Set ADSR parameters
   adsr_setAttack(0, attack);
   adsr_setDecay(0, decay, sustain_level);  // decay speed and target level
   adsr_setSustain(0, sustain_timer_fractional);  // fractional sustain duration
   adsr_setRelease(0, release);
   
   // Activate voice
   adsr_activateVoice(0, 63);
}

void setup_voice()
{
   VERA.control      = 0;
   VERA.address      = 0xf9c0;        // voice 0
   VERA.address_hi   = VERA_INC_1 + 1;
   VERA.data0        = 0;              // freq lo
   VERA.data0        = 0;              // freq hi
   VERA.data0        = STEREO_VOLUME(30);  // volume
   VERA.data0        = wave_values[wave_index];  // waveform
}

void main()
{
   unsigned char key;
   
   set_PET_font();
   clrscr();
   
   cprintf("loading adsr library...\r\n");
   adsr_install();
   
   cprintf("initializing voice...\r\n");
   setup_voice();
   
   cprintf("enabling adsr handler...\r\n");
   adsr_setHandler(ADSR_ON);
   
   sleep(1);
   
   display_menu();
   
   while (1) {
      if (kbhit()) {
         key = cgetc();
         
         switch (key) {
            case 'q':
            case 'Q':
               clrscr();
               return;
            
            case ' ':
               trigger_note();
               break;
            
            // Waveform cycle
            case 'w':
            case 'W':
               wave_index = (wave_index + 1) & 3;
               setup_voice();
               display_menu();
               break;
            
            // Frequency up/down
            case 'h':
            case 'H':
               if (note_index < 53) note_index++;
               display_menu();
               break;
            case 'n':
            case 'N':
               if (note_index > 0) note_index--;
               display_menu();
               break;
            
            // Attack up/down
            case 'A':
               if (attack < 255) attack++;
            case 'a':
               if (attack < 255) attack++;
               display_menu();
               break;
            case 'Z':
               if (attack > 0) attack--;
            case 'z':
               if (attack > 0) attack--;
               display_menu();
               break;
            
            // Decay up/down
            case 'S':
               if (decay < 255) decay++;
            case 's':
               if (decay < 255) decay++;
               display_menu();
               break;
            case 'X':
               if (decay > 0) decay--;
            case 'x':
               if (decay > 0) decay--;
               display_menu();
               break;
            
            // Sustain level up/down
            case 'D':
               if (sustain_level < 63) sustain_level++;
            case 'd':
               if (sustain_level < 63) sustain_level++;
               display_menu();
               break;
            case 'C':
               if (sustain_level > 0) sustain_level--;
            case 'c':
               if (sustain_level > 0) sustain_level--;
               display_menu();
               break;
            
            // Sustain timer fractional up/down
            case 'F':
               if (sustain_timer_fractional < 255) sustain_timer_fractional++;
            case 'f':
               if (sustain_timer_fractional < 255) sustain_timer_fractional++;
               display_menu();
               break;
            case 'V':
               if (sustain_timer_fractional > 0) sustain_timer_fractional--;
            case 'v':
               if (sustain_timer_fractional > 0) sustain_timer_fractional--;
               display_menu();
               break;
            
            // Release up/down
            case 'G':
               if (release < 255) release++;
            case 'g':
               if (release < 255) release++;
               display_menu();
               break;
            case 'B':
               if (release > 0) release--;
            case 'b':
               if (release > 0) release--;
               display_menu();
               break;
         }
      }
      
      waitvsync();
   }
}
