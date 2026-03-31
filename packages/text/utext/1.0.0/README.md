# text.utext

Compact text decode utilities for constrained 8-bit targets.

## Current API

- `char* decodeUtext(const unsigned char* inbuf, char* outbuf)`
- `char* decodeUtextbuf(const unsigned char* buf, char* outbuf)`

`decodeUtextbuf()` currently forwards to `decodeUtext()`.

## 65C02 Assembly Feasibility Notes

This module is a good candidate for a 65C02 assembly implementation wrapped by a cc65 C API.

Why it is a good fit:
- Tight byte-oriented loop.
- Simple bit slicing and arithmetic.
- Small state footprint.
- No libc dependency required for the decode hot path.

Core decode operations:
- `b2 = b0 & 31`
- `b3 = (b0 >> 5) + ((b1 & 3) << 3)`
- `b4 = (b1 & 127) >> 2`
- `b5 = b1 >> 7`
- Decode until `b5 == 1`.

## cc65 Integration Plan

1. Keep `include/utext.h` unchanged.
2. Implement `decodeUtext` in an assembly source file (for example `src/platform/x16/utext_decode.s`).
3. Export the symbol using cc65-compatible calling conventions.
4. Optionally keep a C wrapper for helper entry points.

## Expected Results

- Faster decode loop than the C version.
- Likely smaller code size when using zero-page temporaries and a compact lookup flow.

## Main Gotchas

- Confirm cc65 argument passing and return conventions for pointer arguments.
- Keep internal decode state local to avoid accidental non-reentrant behavior.
- Verify bank visibility if decoding from banked RAM addresses on X16.

## Status

Assembly implementation is not started yet. This document exists to capture the plan for a future pass.
