# CONVERT_ME: system.timer 1.0.0

Status: Migration complete

Classification:
- `src/platform/host/timer.c`: POSIX `clock_gettime` implementation
- `src/platform/x16/timer.c`: cc65 `clock_gettime` implementation (cc65 provides this)

Target layout:
- `src/platform/host/timer.c`
- `src/platform/x16/timer.c`

TODO:
- [x] Move current `src/timer.c` to `src/platform/host/timer.c`
- [x] Implement x16 timer backend (`clock_gettime` available in cc65; compiled OK with cl65)
- [x] `getMillis()` semantics documented: returns millisecond component only (0–999)
- [ ] Update dependent packages (notably `audio/psg`) to pick correct backend
- [ ] On-device validation of `pause_jiffies` timing accuracy on CX16
