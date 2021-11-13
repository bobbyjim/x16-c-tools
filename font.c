#include <cbm.h>

#include "font.h"

void loadFont(char* filename)
{
    cbm_k_setnam(filename);
    cbm_k_setlfs(0,8,0);
    cbm_k_load(2, 0x0f800); // 2 = VERA
}
