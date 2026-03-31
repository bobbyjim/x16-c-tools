# CONVERT_ME: graphics.pixobject 1.0.0

Status: Needs structure review

Classification:
- package appears tool/data-oriented (Perl tools, data assets), not a C runtime library

Target layout:
- likely no `src/core` or `src/platform/*` split required unless runtime C code is added

TODO:
- [x] Confirm this package intentionally has no C runtime source — confirmed, tools/ contains Perl utilities only
- [ ] Optionally split tools by host platform under `tools/host/` if desired
- [x] Document that this package is tool/asset centric

Status: No src/ migration needed. This is a tools/data-only package.
