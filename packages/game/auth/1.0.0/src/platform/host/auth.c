#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "auth.h"

static void seed_once(void)
{
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
}

auth_result auth_challenge(void)
{
    uint32_t code;
    char input[16];
    char *p;

    printf("\n");
    printf("  AUTHENTICATION REQUIRED\n");
    printf("  -----------------------\n");
    printf("  Press return key to receive your security code.\n");
    fflush(stdout);

    if (fgets(input, (int)sizeof(input), stdin) == NULL) {
        return AUTH_FAIL;
    }

    seed_once();
    code = (uint32_t)((unsigned int)rand() % 1000000u);

    printf("  Security code: %06u\n", (unsigned int)code);
    printf("\n");
    printf("  Enter code to continue: ");
    fflush(stdout);

    if (fgets(input, (int)sizeof(input), stdin) == NULL) {
        return AUTH_FAIL;
    }

    for (p = input; *p && *p != '\n' && *p != '\r'; p++) {}
    *p = '\0';

    return auth_check(code, input);
}
