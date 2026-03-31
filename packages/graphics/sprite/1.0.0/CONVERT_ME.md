# CONVERT_ME: graphics.sprite 1.0.0

Status: Not converted

Classification:
- platform/x16: `src/sprite.c` (VERA + KERNAL calls)

Target layout:
- `src/platform/x16/sprite.c`

TODO:
- [x] Move `src/sprite.c` -> `src/platform/x16/sprite.c`
- [x] Verify include paths remain valid (compiled OK with `-I include/`)
- [x] Add `src/platform/host/sprite.c` stub (no-op with `printf` traces, compiled OK with `gcc`)
- [ ] Validate sprite load/define/refresh flows on CX16

Status: Migration complete. Host stub added. On-device smoke test pending.
