#include <cbm.h>
#include <cx16.h>

#include "sprite.h"
#include "PSG.h"

uint8_t spritenum = 1;
SpriteDefinition sprdef;

void main()
{
   cbm_k_bsout(CH_FONT_UPPER);

   sprite_loadToVERA("tartane-32x32.bin", 0x4400);

   sprdef.mode              = SPRITE_MODE_8BPP;
   sprdef.block             = 0x4400;
   sprdef.collision_mask    = 0;
   sprdef.layer             = SPRITE_LAYER_1;
   sprdef.dimensions        = SPRITE_32_BY_32;
   sprdef.palette_offset    = 0;
   sprdef.x                 = SPRITE_X_SCALE(300);
   sprdef.y                 = SPRITE_Y_SCALE(200);

   vera_sprites_enable(1); // cx16.h
   sprite_define(spritenum, &sprdef);
}
