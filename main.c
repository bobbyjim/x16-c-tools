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

void main()
{
   cbm_k_bsout(CH_FONT_UPPER);

   cprintf( "%s\r\n", decodeZtextbuf( buf1, outbuf ) );
   cprintf( "%s\r\n", decodeZtextbuf( buf2, outbuf ) );

   cputs("press a key");
   cgetc();

   loadFont("img/petfont.bin");
   bgcolor(COLOR_BLACK);
   textcolor(COLOR_GREEN);
   clrscr();
   _randomize();

   cputsxy(8,1," **** commander x16 sprite and psg demo ****");
   cputsxy(8,3," 512 high ram - rom version r38");
   cputsxy(8,5," 38655 basic bytes free");
   cclearxy(0,8,80);
   cclearxy(0,9,80);

   sprite_loadToVERA("img/asteroid-4-32x16.bin", 0x4000);
   sprite_loadToVERA("img/star-4-32x16.bin", 0x4400);
   sprite_loadToVERA("img/x16-logo-64.bin", 0x5000);

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

   voice.frequency  = getTunedNote(50);
   voice.channel    = PSG_CHANNEL_BOTH;
   voice.volume     = PSG_VOLUME_KNOB_11;
   voice.waveform   = PSG_WAVE_NOISE;
   voice.pulseWidth = 0;
   ADSR_ENVELOPE(1)->attack  = 0;
   ADSR_ENVELOPE(1)->decay   = 10;
   ADSR_ENVELOPE(1)->sustain = 0;
   ADSR_ENVELOPE(1)->release = 10;
   runVoiceWithEnvelope( 1, &voice );

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
