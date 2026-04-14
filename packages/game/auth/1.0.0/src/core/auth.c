#include <stdint.h>
#include <stddef.h>

#include "auth.h"

auth_result auth_check(uint32_t code, const char *input)
{
    uint32_t entered = 0;
    int i;

    if (input == NULL) {
        return AUTH_FAIL;
    }

    for (i = 0; i < 6; i++) {
        if (input[i] < '0' || input[i] > '9') {
            return AUTH_FAIL;
        }
        entered = entered * 10u + (uint32_t)(input[i] - '0');
    }

    if (input[6] != '\0') {
        return AUTH_FAIL;
    }

    return (entered == code) ? AUTH_OK : AUTH_FAIL;
}
