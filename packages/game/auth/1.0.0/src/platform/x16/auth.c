#include <stdlib.h>
#include <stdint.h>
#include <conio.h>

#include "auth.h"

static void seed_once(void)
{
    static unsigned char seeded = 0;
    if (!seeded) {
        /* Use the jiffy clock low byte as entropy */
        srand((unsigned int)(*(volatile unsigned char *)0x0012));
        seeded = 1;
    }
}

auth_result auth_challenge(void)
{
    uint32_t code;
    char input[7];
    unsigned char i;
    char c;

    cputs("\r\n");
    cputs("  AUTHENTICATION REQUIRED\r\n");
    cputs("  -----------------------\r\n");
    cputs("  Press return key to receive your security code.\r\n");
    cgetc();

    seed_once();
    code = (uint32_t)((unsigned int)rand() % 1000000u);

    cprintf("  Security code: %06u\r\n", (unsigned int)code);
    cputs("\r\n");
    cputs("  Enter code to continue: ");

    i = 0;
    while (i < 6) {
        c = cgetc();
        if (c == '\r' || c == '\n') {
            break;
        }
        if (c >= '0' && c <= '9') {
            cputc(c);
            input[i++] = c;
        }
    }
    input[i] = '\0';

    cputs("\r\n");

    return auth_check(code, input);
}
