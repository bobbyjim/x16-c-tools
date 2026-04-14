#ifndef _AUTH_H_
#define _AUTH_H_

#include <stdint.h>

typedef enum {
    AUTH_OK   =  0,
    AUTH_FAIL = -1
} auth_result;

/*
 * Verify that a string input matches the expected 6-digit code.
 * input must be exactly 6 decimal digit characters followed by '\0'.
 * Returns AUTH_FAIL if input is NULL, not exactly 6 digits, or does not match.
 */
auth_result auth_check(uint32_t code, const char *input);

/*
 * Run the full interactive authentication challenge:
 * generate a 6-digit code, display it, prompt for user input, verify.
 * Returns AUTH_OK if the user entered the correct code, AUTH_FAIL otherwise.
 */
auth_result auth_challenge(void);

#endif
