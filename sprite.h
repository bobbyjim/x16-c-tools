#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stdint.h>

#define		SPRITE_MODE_8BPP			128
#define		SPRITE_MODE_4BPP			0

//
//  64 x 64 is bits     1111xxxx
//  64 x  8 is bits     0011xxxx
//  32 x 32 is bits     1010xxxx
//  16 x 16 is bits     0101xxxx
//  16 x  8 is bits     0001xxxx
//   8 x  8 is bits     0000xxxx

#define 	SPRITE_8_BY_8				0
#define		SPRITE_16_BY_8				(0   +  16)
#define		SPRITE_16_BY_16				(64  +  16)
#define		SPRITE_32_BY_8				(0   +  32)
#define		SPRITE_32_BY_32				(128 +  32)
#define		SPRITE_64_BY_8				(0   +  48)
#define		SPRITE_64_BY_64				(192 +  48)

#define		SPRITE_DISABLED				0
#define	    SPRITE_LAYER_BACKGROUND		(1 << 2)
#define		SPRITE_LAYER_0				(2 << 2)
#define		SPRITE_LAYER_1				(3 << 2)

#define	    SPRITE_SET_IMAGE	        0xFEF0
#define     SPRITE_MOVE		            0xFEF3
#define     SPRITE_X_SCALE(x)           (x<<5)
#define     SPRITE_Y_SCALE(y)           (y<<5)


#define		VERA_SPRITE_AREA_BOTTOM		0x4000
#define		VERA_SPRITE_AREA_TOP		0xcfff

//
//  Stuff you shouldn't care about
//
#define		SPRITE_REGISTER_OFFSET(spritenum)	(spritenum << 3)
#define 	SPRITE_REGISTERS(spritenum)	((spritenum << 3) + 0xfc00)
#define     SPRITE_BLOCK_HI(addr)      ((addr >> 13) & 0x1f)
#define     SPRITE_BLOCK_LO(addr)      ((addr >> 5) & 0xff)

//
//  A note on positions:
//  15 bits is an abuse, unless you use it fractionally.
//  Let's shift it by 5 bits before using it.
//

typedef struct {
	uint16_t block; 
	uint8_t  mode;
    uint8_t  collision_mask;
    uint8_t  layer;
	uint8_t  dimensions;      
	int8_t   palette_offset;
	int      x : 15;
	int      y : 15;
} SpriteDefinition;

typedef struct {
	int x    : 15;
	int flip : 1;
	int y    : 15;
} Position;

/*
typedef struct {
	int x;
	int y;
} SmallPosition;
*/

void sprite_loadToVERA(char *filename, uint16_t address);

void sprite_define(uint8_t spritenum, SpriteDefinition *sprdef);
void sprite_define_in_bank(uint8_t spritenum, uint8_t sprite_ram_bank_num, SpriteDefinition *sprdef);
void sprite_changeBlock(uint8_t spritenum, SpriteDefinition *sprdef);
void sprite_pos(uint8_t spritenum, Position* pos);
void sprite_flip(uint8_t spritenum, uint8_t flip);
void sprite_refresh(uint8_t sprite_ram_bank_num);

#endif
