#ifndef _SYSTEM_FONT_H_
#define _SYSTEM_FONT_H_

enum {
    FONT_C64 = 0,
    FONT_PET = 4
};

void setSystemFont(unsigned char font_id);
void setFontPET(void);

#endif /* _SYSTEM_FONT_H_ */