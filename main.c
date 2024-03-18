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
#define      NUM_ASTEROIDS      32
#define      NUM_SPRITES        (NUM_ASTEROIDS+2)
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
   cputsxy(8,3," rom version r46");
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
   for (i=2; i<NUM_ASTEROIDS+2; ++i)
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
   sprite_define(1, &sprdef[0]);
}

void demoZtext()
{
   cprintf( "text 1 (8 bytes packed): %s\r\n\r\n", decodeUtextbuf( buf1, outbuf ) );
   cprintf( "text 2 (8 bytes packed): %s\r\n\r\n", decodeUtextbuf( buf2, outbuf ) );
}

void demoPSG()
{
   //ping(8000);

   // This sounds a lot like sea travel in 7 Cities of Gold.

//   unsigned i = 30000;
//   unsigned char backwards = 0;
//   unsigned lowpoint = 4000;
//   unsigned highpoint = 30000;
//
//   cprintf("press a key to exit psg demo");
//   while(!kbhit())
//   {
//      if (backwards) i -= 2;
//      else i += 1;
//
//      waves(i);
//      if ( i > highpoint ) {
//         backwards = 1;
//         lowpoint = 2000 + rand() / 10;
//      }
//      if ( i < lowpoint ) {
//         backwards = 0;
//         highpoint = 10000 + rand() / 2;
//      }
//   }
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

void show_ship(uint8_t spriteNum, SpriteDefinition* obj, uint8_t ship_facing)
{
   switch(ship_facing)
   {
      case 0:   obj->block = SHIP_0;           obj->flip_horiz = 0;         obj->flip_vert  = 0;     break;
      case 1:   obj->block = SHIP_22;          obj->flip_horiz = 0;         obj->flip_vert  = 0;     break;
      case 2:   obj->block = SHIP_45;          obj->flip_horiz = 0;         obj->flip_vert  = 0;     break;
      case 3:   obj->block = SHIP_68;          obj->flip_horiz = 0;         obj->flip_vert  = 0;     break;
      case 4:   obj->block = SHIP_90;          obj->flip_horiz = 0;         obj->flip_vert  = 0;     break;
      case 5:   obj->block = SHIP_68;          obj->flip_horiz = 0;         obj->flip_vert  = 1;     break;
      case 6:   obj->block = SHIP_45;          obj->flip_horiz = 0;         obj->flip_vert  = 1;     break;
      case 7:   obj->block = SHIP_22;          obj->flip_horiz = 0;         obj->flip_vert  = 1;     break;
      case 8:   obj->block = SHIP_0;           obj->flip_horiz = 0;         obj->flip_vert  = 1;     break;
      case 9:   obj->block = SHIP_22;          obj->flip_horiz = 1;         obj->flip_vert  = 1;     break;
      case 10:  obj->block = SHIP_45;          obj->flip_horiz = 1;         obj->flip_vert  = 1;     break;
      case 11:  obj->block = SHIP_68;          obj->flip_horiz = 1;         obj->flip_vert  = 1;     break;
      case 12:  obj->block = SHIP_90;          obj->flip_horiz = 1;         obj->flip_vert  = 1;     break;
      case 13:  obj->block = SHIP_68;          obj->flip_horiz = 1;         obj->flip_vert  = 0;     break;
      case 14:  obj->block = SHIP_45;          obj->flip_horiz = 1;         obj->flip_vert  = 0;     break;
      case 15:
      default:  obj->block = SHIP_22;          obj->flip_horiz = 1;         obj->flip_vert  = 0;     break;
   }
   sprite_changeBlock( spriteNum, obj );
}

void accelerate_ship(uint8_t spriteNum, SpriteDefinition* obj, uint8_t ship_facing)
{
   int dx = 0, dy = 0;

   switch(ship_facing)
   {
      default:
      case  0: dx =    0; dy =  -32; break; // 0
      case  1: dx =   11; dy =  -29; break; // 22
      case  2: dx =   22; dy =  -22; break; // 45
      case  3: dx =   29; dy =  -11; break; // 68
      case  4: dx =   31; dy =    0; break; // 90
      case  5: dx =   29; dy =   11; break; // 112
      case  6: dx =   22; dy =   22; break; // 135
      case  7: dx =   11; dy =   29; break; // 158
      case  8: dx =    0; dy =   31; break; // 180
      case  9: dx =  -11; dy =   29; break; // 202
      case 10: dx =  -22; dy =   22; break; // 225
      case 11: dx =  -29; dy =   11; break; // 248
      case 12: dx =  -31; dy =    0; break; // 270
      case 13: dx =  -29; dy =  -11; break; // 292
      case 14: dx =  -22; dy =  -22; break; // 315
      case 15: dx =  -11; dy =  -29; break; // 338
   }
   if ( dy < 0 && obj->y > SPRITE_Y_SCALE(0))   obj->dy += dy;
   if ( dy > 0 && obj->y < SPRITE_Y_SCALE(500)) obj->dy += dy;
   if ( dx < 0 && obj->x > SPRITE_X_SCALE(0))   obj->dx += dx;
   if ( dx > 0 && obj->x < SPRITE_X_SCALE(500)) obj->dx += dx;
}

void demoSprites()
{
   uint8_t i;
   uint8_t ship_angle = 0;

   for(i=2; i<NUM_ASTEROIDS+2; ++i)
   {
      sprdef[i].dx = -rand() % 1000;
      sprdef[i].dy = -rand() % 100;
   }

   tmp = &sprdef[0];

   for(;;)
   {
      for(i=2; i <NUM_ASTEROIDS+2; ++i)
      {
         gravity(i, &sprdef[i]);
      }

      // Update ship

      show_ship(1, tmp, ship_angle);
      gravity(1, tmp);

      if (kbhit()) switch (cgetc())
      {
         case 'a':
            if (ship_angle == 0) ship_angle = 16;
            --ship_angle;
            break;
         case 'd':
            ++ship_angle;
            if (ship_angle == 16) ship_angle = 0;
            break;
         case ' ':
            accelerate_ship(10, tmp, ship_angle);
            break;
//         case 'w': 
//            if (tmp->y > SPRITE_Y_SCALE(0)) tmp->dy -= 32;
//            break;
//         case 's': 
//            if (tmp->y < SPRITE_Y_SCALE(500)) tmp->dy += 32;
//            break;
//         case 'a':
//            if (tmp->x > SPRITE_X_SCALE(0)) tmp->dx -= 32;
//            break; 
//         case 'd':
//            if (tmp->x < SPRITE_X_SCALE(500)) tmp->dx += 32;
//            break;
//         default: 
//            break;
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
