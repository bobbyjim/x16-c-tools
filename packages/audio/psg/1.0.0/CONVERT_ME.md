# CONVERT_ME: audio.psg 1.0.0

Status: Migration complete

Classification:
- `src/platform/x16/PSG.c`: VERA PSG hardware implementation, depends on `timer.h`
- `src/platform/host/PSG.c`: Host stub (printf traces), depends on `timer.h`

Notes:
- `ADSR_ENVELOPE(x)` macro uses a fixed X16 memory address (`0x0400`); host stub does not exercise it
- `runVoiceWithEnvelope` on host is a no-op trace; timing loop via `pause_jiffies` is not simulated
- `tunedNotes[]` table is duplicated in host stub (static) since it is pure data with no platform dependency

TODO:
- [x] Move `src/PSG.c` -> `src/platform/x16/PSG.c`
- [x] Add `src/platform/host/PSG.c` stub (compiled OK with `gcc`)
- [ ] On-device validation of ADSR envelope shapes on CX16
- [ ] Consider extracting `tunedNotes[]` and `getTunedNote()` to `src/core/` if reuse across platforms is desired
