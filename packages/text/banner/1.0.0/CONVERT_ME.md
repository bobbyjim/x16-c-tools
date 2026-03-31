# CONVERT_ME: text.banner 1.0.0

Status: Migration complete

Classification:
- `src/platform/x16/banner.s`: Assembly runtime loaded at `$A000`, bank 1
- `src/platform/x16/splash.c` + `splash.h`: C wrapper that calls banner via `_sys()` at `$A000`
- `examples/platform/x16/test-splash.c`: Demo driver showing `splashBanner()` usage (no assertions)
- `test/platform/x16/BANNER-TEST.BAS`: On-device BASIC test

Notes:
- `splash.c`/`splash.h` were previously under `test/` but are genuinely usable runtime code; promoted to `src/platform/x16/`
- `test-splash.c` is an example/demo (has `main()`, no assertions); moved to `examples/platform/x16/`
- Empty `tools/` directory removed
- Assembly build: `cl65 -t cx16 -C cx16-asm.cfg -o BANNER-FONT.BIN src/platform/x16/banner.s`

TODO:
- [x] Move `src/banner.s` -> `src/platform/x16/banner.s`
- [x] Move `test/BANNER-TEST.BAS` -> `test/platform/x16/BANNER-TEST.BAS`
- [x] Promote `splash.c`/`splash.h` -> `src/platform/x16/` (compiled OK with cl65)
- [x] Move `test-splash.c` -> `examples/platform/x16/`
- [x] Remove empty `tools/` directory
- [ ] On-device validation of banner rendering and splash wrapper on CX16
