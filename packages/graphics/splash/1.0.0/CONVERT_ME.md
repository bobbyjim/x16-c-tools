# CONVERT_ME: graphics.splash 1.0.0

Status: Not converted

Classification:
- platform/x16: `src/splash.c` (`cx16.h`, `6502.h`, `conio.h`)

Target layout:
- `src/platform/x16/splash.c`

TODO:
- [x] Move `src/splash.c` -> `src/platform/x16/splash.c`
- [x] Verify include paths remain valid (compiled OK with `-I include/`)
- [x] Add `src/platform/host/splash.c` stub (no-op with `printf` trace, compiled OK with `gcc`)
- [ ] Run smoke test for login/logo rendering on CX16

Status: Migration complete. Host stub added. On-device smoke test pending.
