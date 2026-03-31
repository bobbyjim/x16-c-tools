# CONVERT_ME: system.font 1.0.0

Status: Not converted

Classification:
- platform/x16: `src/font.c` (`_sys` KERNAL call)

Target layout:
- `src/platform/x16/font.c`

TODO:
- [x] Move `src/font.c` -> `src/platform/x16/font.c`
- [x] Verify include paths remain valid (compiled OK with `-I include/`)
- [x] Add `src/platform/host/font.c` stub (no-op with `printf` trace, compiled OK with `gcc`)
- [ ] Smoke-test PET/system font switching on CX16

Status: Migration complete. Host stub added. On-device smoke test pending.
