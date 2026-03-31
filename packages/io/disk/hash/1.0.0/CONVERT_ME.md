# CONVERT_ME: io.disk.hash 1.0.0

Status: Migration complete

Classification:
- `src/core/hash.c`: File-backed hash with two-slot cache. Uses `stdio.h`, `dirent.h`, `string.h`,
  `ctype.h` — all available in cc65 for cx16. Not host-only; correct in `src/core/`.

Notes:
- Initially suspected of being host-only due to `stdio`/`dirent` usage, but cc65 provides both
  for cx16 (CBM DOS-backed). Verified: compiles clean with `cl65 -t cx16`.
- No platform split is needed unless a target without stdio/dirent support is added later.
- Tests are in `test/platform/host/tests.c`; 29/29 passing.

TODO:
- [x] Verify `src/core/hash.c` compiles with cl65 -t cx16 (confirmed clean)
- [x] Move `test/tests.c` -> `test/platform/host/tests.c` (29/29 still passing)
- [ ] On-device validation of file read/write/list on CX16 CBM DOS
