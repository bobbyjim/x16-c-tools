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
#define      NUM_SPRITES        16
#define      STAR_X             300
#define      STAR_Y             220

uint8_t spritenum = 1;
SpriteDefinition sprdef;
Voice voice;
Position pos[NUM_SPRITES+1];
Position *tmp;
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
   sprite_loadToVERA("asteroid-4-32x16.bin", 0x4000);
   sprite_loadToVERA("star-4-32x16.bin", 0x4400);
   sprite_loadToVERA("x16-logo-64.bin", 0x5000);

   sprdef.mode              = SPRITE_MODE_8BPP;
   sprdef.block             = 0x4000;
   sprdef.collision_mask    = 0;
   sprdef.layer             = SPRITE_LAYER_1;
   sprdef.dimensions        = SPRITE_16_BY_16;
   sprdef.palette_offset    = 0;
   sprdef.x                 = SPRITE_X_SCALE(200);
   sprdef.y                 = SPRITE_Y_SCALE(100);

   vera_sprites_enable(1); // cx16.h
   for (spritenum=1; spritenum<=NUM_SPRITES; ++spritenum)
   {
      sprite_define(spritenum, &sprdef);
      pos[spritenum].x  = SPRITE_X_SCALE(100 + rand() % 400);
      pos[spritenum].y  = SPRITE_Y_SCALE(100 + rand() % 200);
      pos[spritenum].dx = 4 + rand() % VELOCITY;
      pos[spritenum].dy = 4 + rand() % VELOCITY;
   }

   sprdef.block            = 0x4400;
   sprdef.x                 = SPRITE_X_SCALE(STAR_X);
   sprdef.y                 = SPRITE_Y_SCALE(STAR_Y);
   sprite_define(127, &sprdef);

   sprdef.block            = 0x5000;
   sprdef.dimensions       = SPRITE_64_BY_64;
   sprdef.x                = SPRITE_X_SCALE(0);
   sprdef.y                = SPRITE_Y_SCALE(0);
   sprite_define(0, &sprdef);
}

void demoZtext()
{
   cprintf( "\r\n%s\r\n\r\n", decodeZtextbuf( buf1, outbuf ) );
   cprintf( "\r\n%s\r\n\r\n", decodeZtextbuf( buf2, outbuf ) );
}

void demoPSG()
{
   voice.frequency  = getTunedNote(50);
   voice.channel    = PSG_CHANNEL_BOTH;
   voice.volume     = PSG_VOLUME_KNOB_11;
   voice.waveform   = PSG_WAVE_NOISE;
   voice.pulseWidth = 0;
   ADSR_ENVELOPE(1)->attack  = 300;
   ADSR_ENVELOPE(1)->decay   = 100;
   ADSR_ENVELOPE(1)->sustain = 300;
   ADSR_ENVELOPE(1)->release = 200;
   runVoiceWithEnvelope( 1, &voice );
}

void demoSprites()
{
   for(;;)
   {
      for(spritenum = 1; spritenum <= NUM_SPRITES; ++spritenum)
      {
         tmp = &pos[spritenum];
         sprite_pos(spritenum, tmp);
         tmp->x += tmp->dx >> 4;
         tmp->y += tmp->dy >> 4;

//         distance = usqrt4((tmp->x - STAR_X) * (tmp->x - STAR_X)
//                  + (tmp->y - STAR_Y) * (tmp->y - STAR_Y));


         if (tmp->x < SPRITE_X_SCALE(STAR_X)) tmp->dx++;
         if (tmp->x > SPRITE_X_SCALE(STAR_X)) tmp->dx--;
         if (tmp->y < SPRITE_Y_SCALE(STAR_Y)) tmp->dy++;
         if (tmp->y > SPRITE_Y_SCALE(STAR_Y)) tmp->dy--;

      }

//      pause_jiffies(1);
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
