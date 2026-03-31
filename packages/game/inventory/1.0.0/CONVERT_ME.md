# CONVERT_ME: game.inventory 1.0.0

Status: Migration complete

Classification:
- `src/core/inventory.c`: Pure logic, `stdint.h` only — no platform dependencies
- `examples/platform/host/cli.c`: Interactive host driver demonstrating the API; has `main()`, not a library file

Notes:
- `cli.c` was originally placed under `src/platform/host/` but does not implement any inventory function.
  It calls the API interactively via stdin and belongs in `examples/`.
- No platform/x16 source is needed: inventory logic is pure core.
- No host platform stub is needed: there is no hardware-dependent surface in this package.

TODO:
- [x] Move `src/inventory.c` -> `src/core/inventory.c`
- [x] Move `src/platform/host/cli.c` -> `examples/platform/host/cli.c`
- [x] Verify examples/platform/host/cli.c compiles and links with src/core/inventory.c (OK)
- [x] Move `test/tests.c` -> `test/platform/host/tests.c`
- [ ] Add assertion-based correctness tests to `test/platform/host/`
