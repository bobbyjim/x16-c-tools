#include <stdio.h>
#include "sprite.h"

void sprite_loadToVERA(char *filename, uint16_t address)
{
    printf("[sprite stub] sprite_loadToVERA(\"%s\", 0x%04x)\n", filename, address);
}

void sprite_refresh(void)
{
    printf("[sprite stub] sprite_refresh()\n");
}

void sprite_define(uint8_t spritenum, SpriteDefinition *sprdef)
{
    printf("[sprite stub] sprite_define(%d)\n", spritenum);
}

void sprite_changeBlock(uint8_t spritenum, SpriteDefinition *sprdef)
{
    printf("[sprite stub] sprite_changeBlock(%d)\n", spritenum);
}

void sprite_pos(uint8_t spritenum, SpriteDefinition *pos)
{
    printf("[sprite stub] sprite_pos(%d, x=%d, y=%d)\n", spritenum, pos->x, pos->y);
}

void sprite_flip(uint8_t spritenum, SpriteDefinition *pos)
{
    printf("[sprite stub] sprite_flip(%d)\n", spritenum);
}

void sprite_disable(uint8_t spritenum)
{
    printf("[sprite stub] sprite_disable(%d)\n", spritenum);
}
