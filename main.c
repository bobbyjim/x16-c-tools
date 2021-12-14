#include <cbm.h>
#include <cx16.h>
#include <unistd.h>
#include <stdlib.h>
#include <conio.h>

#include "sprite.h"
#include "PSG.h"
#include "timer.h"
#include "font.h"
#include "math.h"
#include "ztext.h"

#define      VELOCITY           300
#define      NUM_ASTEROIDS      2
#define      NUM_SPRITES        (NUM_ASTEROIDS+1)
#define      STAR_X             300
#define      STAR_Y             220

SpriteDefinition sprdef[NUM_SPRITES+1];
SpriteDefinition* tmp;
Voice voice;
unsigned distance;

unsigned char buf1[8] = { 162, 12, 171, 79, 224, 62, 146, 145 };
unsigned char buf2[8] = { 137, 34, 79,  58, 46,  76, 244, 201 };
char outbuf[256];

void setupScreen()
{
   loadFont("petfont.bin");
   bgcolor(COLOR_BLACK);
   textcolor(COLOR_GREEN);
}

void splash(char *msg)
{
   clrscr();
   cprintf("\r\n         ***** %s *****\r\n\r\n", msg);
   cputsxy(8,3," rom version r38");
   cputsxy(8,5," proof of concept");
   gotoxy(0,10);
}

void setupSprites()
{
   uint8_t i;

   sprite_loadToVERA("x16-logo-64.bin",         0x4000);
   sprite_loadToVERA("asteroid-4-32x16.bin",    0x5000);
   sprite_loadToVERA("star-4-32x16.bin",        0x5200);
   sprite_loadToVERA("ship1-8bpp-32x32.bin",    0x5800);
   sprite_loadToVERA("ship1-8bpp-32x32-22.bin", 0x5c00);
   sprite_loadToVERA("ship1-8bpp-32x32-45.bin", 0x6000);
   sprite_loadToVERA("ship1-8bpp-32x32-68.bin", 0x6400);
   sprite_loadToVERA("ship1-8bpp-32x32-90.bin", 0x6800);
   
   vera_sprites_enable(1); // cx16.h

   //
   //  Define the asteroid sprites
   //
   for (i=1; i<=NUM_ASTEROIDS; ++i)
   {
      sprdef[i].mode              = SPRITE_MODE_8BPP;
      sprdef[i].block             = 0x5000;
      sprdef[i].collision_mask    = 0;
      sprdef[i].layer             = SPRITE_LAYER_1;
      sprdef[i].dimensions        = SPRITE_16_BY_16;
      sprdef[i].palette_offset    = 0;
      sprdef[i].x                 = SPRITE_X_SCALE(100 + rand() % 400);
      sprdef[i].y                 = SPRITE_Y_SCALE(100 + rand() % 200);
      sprdef[i].dx                = 4 + rand() % VELOCITY;
      sprdef[i].dy                = 4 + rand() % VELOCITY;
      sprdef[i].flip_horiz        = 1;
      sprite_define(i, &sprdef[i]);
   }

   //
   //  Define the star sprite
   //
   sprdef[0].mode                = SPRITE_MODE_8BPP;
   sprdef[0].block               = 0x5200;
   sprdef[0].collision_mask      = 0;
   sprdef[0].layer               = SPRITE_LAYER_0;
   sprdef[0].dimensions          = SPRITE_16_BY_16;
   sprdef[0].palette_offset      = 0;
   sprdef[0].x                   = SPRITE_X_SCALE(STAR_X);
   sprdef[0].y                   = SPRITE_Y_SCALE(STAR_Y);
   sprite_define(127, &sprdef[0]);

   //
   //  Define the X16 sprite
   //
   sprdef[0].block               = 0x4000;
   sprdef[0].dimensions          = SPRITE_64_BY_64;
   sprdef[0].x                   = SPRITE_X_SCALE(0);
   sprdef[0].y                   = SPRITE_Y_SCALE(0);
   sprite_define(0, &sprdef[0]);

   //
   //  Define the ship sprite
   //
   sprdef[0].block               = 0x5c00;
   sprdef[0].dimensions          = SPRITE_32_BY_32;
   sprdef[0].layer               = SPRITE_LAYER_0;
   sprdef[0].x                   = SPRITE_X_SCALE(100);
   sprdef[0].y                   = SPRITE_Y_SCALE(100);
   sprite_define(10, &sprdef[0]);
}

void demoZtext()
{
   cprintf( "\r\n%s\r\n\r\n", decodeZtextbuf( buf1, outbuf ) );
   cprintf( "\r\n%s\r\n\r\n", decodeZtextbuf( buf2, outbuf ) );
}

void demoPSG()
{
   bang(2000);
   /*
   voice.frequency  = getTunedNote(50);
   voice.channel    = PSG_CHANNEL_BOTH;
   voice.volume     = PSG_VOLUME_KNOB_11;
   voice.waveform   = PSG_WAVE_NOISE;
   voice.pulseWidth = 0;
   ADSR_ENVELOPE(1)->attack  = 10;
   ADSR_ENVELOPE(1)->decay   = 50;
   ADSR_ENVELOPE(1)->sustain = 80;
   ADSR_ENVELOPE(1)->release = 50;
   runVoiceWithEnvelope( 1, &voice );
   */
}

void demoSprites()
{
   uint8_t i;

   for(;;)
   {
      for(i = 1; i <= NUM_ASTEROIDS; ++i)
      {
         tmp = &sprdef[i];
         sprite_pos(i, tmp);
         tmp->x += tmp->dx >> 4;
         tmp->y += tmp->dy >> 4;

//         distance = usqrt4((tmp->x - STAR_X) * (tmp->x - STAR_X)
//                  + (tmp->y - STAR_Y) * (tmp->y - STAR_Y));

         if (tmp->x < SPRITE_X_SCALE(STAR_X)) tmp->dx++;
         else if (tmp->x > SPRITE_X_SCALE(STAR_X)) tmp->dx--;
         if (tmp->y < SPRITE_Y_SCALE(STAR_Y)) tmp->dy++;
         else if (tmp->y > SPRITE_Y_SCALE(STAR_Y)) tmp->dy--;

         if (tmp->dx < 0) tmp->flip_horiz = 1;
         else if (tmp->dx > 0) tmp->flip_horiz = 0;
         if (tmp->dy < 0) tmp->flip_vert  = 0;
         else if (tmp->dy > 0) tmp->flip_vert  = 1;
      }

      // Update ship
      tmp = &sprdef[0];
      sprite_pos(10, tmp);
      tmp->x += tmp->dx;
      tmp->y += tmp->dy;
      if (kbhit()) switch (cgetc())
      {
         case 'w': 
            if (tmp->y > SPRITE_Y_SCALE(0)) tmp->dy--;
            if (tmp->dy < 0) tmp->flip_vert  = 0;
            break;
         case 's': 
            if (tmp->y < SPRITE_Y_SCALE(500)) tmp->dy++;
            if (tmp->dy > 0) tmp->flip_vert  = 1;
            break;
         case 'a':
            if (tmp->x > SPRITE_X_SCALE(0)) tmp->dx--;
            if (tmp->dx < 0) tmp->flip_horiz = 1;
            break; 
         case 'd':
            if (tmp->x < SPRITE_X_SCALE(500)) tmp->dx++;
            if (tmp->dx > 0) tmp->flip_horiz = 0;
            break;
         default: 
            break;
      }

      pause_jiffies(1);
   }
}

void main()
{
   setupScreen();
   setupSprites();

   splash("demo 1: ztext decoder");
   demoZtext();

   cputs("press a key");
   cgetc();
   _randomize();

   splash("demo 2: psg with adsr envelope");
   demoPSG();

   splash("demo 3: sprites");
   demoSprites();
}
