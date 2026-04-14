#include <stdio.h>

#include "auth.h"

int main(void)
{
    printf("Welcome to the vault.\n");

    if (auth_challenge() == AUTH_OK) {
        printf("  Access granted. You may proceed.\n");
    } else {
        printf("  Access denied.\n");
    }

    return 0;
}
