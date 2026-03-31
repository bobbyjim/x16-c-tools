# CONVERT_ME: audio.adsr 1.0.0

Status: Migration complete

Classification:
- `src/platform/x16/adsr1.s`: IRQ-driven ADSR engine assembly, loads to `$A000` bank 1
- `src/platform/x16/ADSR.c`: C API wrapper for the assembly engine
- `examples/platform/x16/example-adsr.c`: Interactive X16 demo driver (no assertions)

Notes:
- No host platform stub — ADSR is tightly coupled to VERA PSG hardware and IRQ vectors
- `example-adsr.c` was previously named `test-adsr.c`; renamed to reflect it is a demo, not a test
- Stale `ADSR.o` object file removed from `src/platform/x16/`
- Assembly build: `cl65 -t cx16 -C cx16-asm.cfg -o ADSR1.PRG src/platform/x16/adsr1.s`

TODO:
- [x] Move `src/ADSR.c` and `src/adsr1.s` -> `src/platform/x16/`
- [x] Rename `test-adsr.c` -> `examples/platform/x16/example-adsr.c`
- [x] Remove stale `ADSR.o` from src/
- [ ] On-device validation of ADSR envelope timing on CX16
