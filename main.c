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
#include "utext.h"

#define      VELOCITY           300
#define      NUM_ASTEROIDS      1
#define      NUM_SPRITES        (NUM_ASTEROIDS+1)
#define      STAR_X             300
#define      STAR_Y             220

#define      SHIP_0             0x5800
#define      SHIP_22            0x5c00
#define      SHIP_45            0x6000
#define      SHIP_68            0x6400
#define      SHIP_90            0x6800

SpriteDefinition sprdef[NUM_SPRITES+1];
SpriteDefinition* tmp;
Voice voice;

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
   sprite_loadToVERA("ship1-8bpp-32x32-90b.bin", 0x6800);
   
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
      sprdef[i].x                 = SPRITE_X_SCALE(rand() % 550);
      sprdef[i].y                 = SPRITE_Y_SCALE(rand() % 450);
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
   sprdef[0].block               = SHIP_0;
   sprdef[0].dimensions          = SPRITE_32_BY_32;
   sprdef[0].layer               = SPRITE_LAYER_0;
   sprdef[0].x                   = SPRITE_X_SCALE(100);
   sprdef[0].y                   = SPRITE_Y_SCALE(100);
   sprite_define(10, &sprdef[0]);
}

void demoZtext()
{
   cprintf( "text 1 (8 bytes packed): %s\r\n\r\n", decodeUtextbuf( buf1, outbuf ) );
   cprintf( "text 2 (8 bytes packed): %s\r\n\r\n", decodeUtextbuf( buf2, outbuf ) );
}

void demoPSG()
{
   pluck(2000);
}

int x_delta, y_delta;

void gravity(uint8_t spritenum, SpriteDefinition* obj)
{
   sprite_pos(spritenum, obj);
   obj->x += obj->dx >> 4;
   obj->y += obj->dy >> 4;

   //
   // deltas range -320 to +320.
   //
   x_delta = (obj->x >> 5) - STAR_X; 
   y_delta = (obj->y >> 5) - STAR_Y;

   //
   // downshift to pull slower
   //
   obj->dx -= x_delta >> 6;
   obj->dy -= y_delta >> 6; 
}

void demoSprites()
{
   uint8_t i;

   for(i=1; i<= NUM_ASTEROIDS; ++i)
   {
      sprdef[i].dx = -rand() % 1000;
      sprdef[i].dy = -rand() % 100;
   }

   tmp = &sprdef[0];

   for(;;)
   {
      for(i = 1; i <= NUM_ASTEROIDS; ++i)
      {
         gravity(i, &sprdef[i]);
      }

      // Update ship

      gravity(10, tmp);

      if (kbhit()) switch (cgetc())
      {
         case 'w': 
            if (tmp->y > SPRITE_Y_SCALE(0)) tmp->dy -= 32;
            tmp->block = SHIP_0;
            tmp->flip_vert = 0;
            sprite_changeBlock( 10, tmp );
            break;
         case 's': 
            if (tmp->y < SPRITE_Y_SCALE(500)) tmp->dy += 32;
            tmp->block = SHIP_0;
            tmp->flip_vert = 1;
            sprite_changeBlock( 10, tmp );
            break;
         case 'a':
            if (tmp->x > SPRITE_X_SCALE(0)) tmp->dx -= 32;
            tmp->block = SHIP_90;
            tmp->flip_horiz = 1;
            sprite_changeBlock( 10, tmp );
            break; 
         case 'd':
            if (tmp->x < SPRITE_X_SCALE(500)) tmp->dx += 32;
            tmp->block = SHIP_90;
            tmp->flip_horiz = 0;
            sprite_changeBlock( 10, tmp );
            break;
         default: 
            break;
      }

      pause_jiffies(2);
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
