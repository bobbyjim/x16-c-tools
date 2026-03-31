# Package Organization Instructions

This file defines the canonical package layout and migration rules for all package work under this directory.

## Canonical Layout

Use this structure when creating or refactoring packages:

- `include/`: Public headers and package API surface.
- `src/core/`: Platform-agnostic runtime logic.
- `src/platform/host/`: Host runtime implementation.
- `src/platform/x16/`: Commander X16 runtime implementation.
- `tools/`: Host-side build/data utilities (not runtime library code).
- `test/platform/host/`: Host-runnable verification.
- `test/platform/x16/`: Target smoke/integration verification.
- `examples/platform/host/`: Host-side usage examples.
- `examples/platform/x16/`: X16 usage examples.
- `data/`: Package assets and generated binary data.

## Classification Rules

- Put code in `src/core/` only if it has no target-specific headers, hardware registers, or platform-only APIs.
- Put code in `src/platform/x16/` if it uses `cx16.h`, `6502.h`, VERA/KERNAL access, memory-mapped I/O, or target assembly routines.
- Put code in `src/platform/host/` if it depends on host-only facilities such as POSIX/stdio behavior not guaranteed on X16.
- Do not place runtime/package API code under `tools/`.
- **Core must not depend on platform-specific behavior, including timing, memory layout, or implicit hardware assumptions.**
- When in doubt, place code in `src/core/` and refactor out platform dependencies explicitly rather than prematurely specializing.

## Port Layer Pattern

Platform-specific implementations must satisfy interfaces defined in `include/` or `src/core/`, not modify or branch core logic.

The informal port layer works as follows:

- `include/` defines the public API (shared across all platforms).
- `src/core/` implements platform-agnostic logic against that API.
- `src/platform/host/` and `src/platform/x16/` each provide implementations of any platform-dependent operations that `src/core/` requires — via the same function signatures declared in `include/` or in a dedicated internal interface (e.g., `src/core/platform.h`).
- Core code never `#ifdef`s on platform; all branching is structural (separate files linked per platform).

## Test and Example Rules

- `test/` is for correctness checks; do not use examples as substitutes for tests.
- Keep host tests as the default when feasible (`test/platform/host/`).
- **All core logic should be validated via host tests; X16 tests verify platform integration, not replace correctness testing.**
- Put target-only smoke/integration checks under `test/platform/x16/`.
- `examples/` is for usage and demos only; they must not be used to validate correctness or replace tests.
- **Examples are authoritative usage references and should reflect idiomatic, current API usage. They must compile and run, but are not used for correctness validation.**
- Avoid mock/demo-only shortcuts in examples; prefer realistic usage patterns.
- Prefer explicit platform folders in examples (`examples/platform/host/`, `examples/platform/x16/`).

### Practical Split Guideline

- If a package currently has only one platform for tests/examples, a flat `test/` or `examples/` is acceptable temporarily.
- As soon as a second platform variant is added, split into `platform/host` and `platform/x16` immediately.

## Data Rules

- `data/` may contain source assets and generated artifacts; generated outputs should be reproducible via `tools/`.

## Migration Conventions

When converting an existing package:

1. Create package-local `CONVERT_ME.md` when work is pending or partial.
2. Move files without changing public headers unless required.
3. Preserve external API names/signatures unless intentionally versioning.
4. Update package docs and build command snippets when source paths change.
5. Mark completed items in `CONVERT_ME.md` with `[x]`.

## Current Naming Decision

Use `core` (not `common`) for shared platform-agnostic runtime logic.

Rationale:
- Better conveys architectural role.
- Reduces risk of `common` becoming a catch-all dumping ground.
- Keeps the split explicit: `core` vs `platform/host` vs `platform/x16`.
