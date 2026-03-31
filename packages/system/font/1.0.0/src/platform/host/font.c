#include <stdio.h>
#include "font.h"

void setSystemFont(unsigned char font_id)
{
    printf("[font stub] setSystemFont(%d)\n", font_id);
}

void setFontPET(void)
{
    setSystemFont(FONT_PET);
}
