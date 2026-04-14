# game.auth

A fake interactive authentication step for games and demos.

Displays an "AUTHENTICATION REQUIRED" gate, waits for the player to press
return, then generates a random six-digit security code and asks them to type
it back in. If they get it right, they proceed. If not, they're denied. It's
completely fake — the code is printed right there in the same console — but it
makes a satisfying obstacle.

## API

```c
/* Verify a string input against a known 6-digit code.
   Input must be exactly 6 decimal digit characters.
   Returns AUTH_FAIL if input is NULL, not 6 digits, or doesn't match. */
auth_result auth_check(uint32_t code, const char *input);

/* Run the full challenge: generate, display, prompt, verify.
   Returns AUTH_OK on success, AUTH_FAIL otherwise. */
auth_result auth_challenge(void);
```

## Example

```c
if (auth_challenge() == AUTH_OK) {
    /* access granted */
}
```

Sample output:

```
  AUTHENTICATION REQUIRED
  -----------------------
   Press return key to receive your security code.
  Security code: 047291

  Enter code to continue: 047291
  Access granted. You may proceed.
```

## Build (host)

From the package root:

```sh
# Tests
cc -std=c99 -Wall -Wextra -pedantic -I include \
   src/core/auth.c test/platform/host/tests.c \
   -o /tmp/auth-tests && /tmp/auth-tests

# Demo
cc -std=c99 -I include \
   src/core/auth.c src/platform/host/auth.c examples/platform/host/demo.c \
   -o /tmp/auth-demo && /tmp/auth-demo
```

## Notes

- The random number is seeded after the player presses return.
- Host uses `time(NULL)` once per process.
- X16 uses the jiffy clock low byte at `$0012`.
- `rand() % 1000000` is used; distribution is not cryptographically uniform,
  which is fine for a game gag.
- Codes are always displayed zero-padded to six digits (e.g. `047291`).
