This package implements a tiny disk-backed symbol table for constrained targets.

Design goals:
- Zero dynamic allocation.
- Persistence via filesystem files.
- Small, predictable API.

Behavior:
- `putSymbol(key, value)` writes `value` to `hash-<key>.txt`.
- `getSymbol(key, value)` reads from `hash-<key>.txt`.
- `showDir()` prints the current directory entries.

Limits:
- Maximum key length: 15 chars (`HASH_KEY_MAX_LEN`).
- Maximum value length: 39 chars (`HASH_VALUE_MAX_LEN`).
- Keys must use: `A-Z`, `a-z`, `0-9`, `_`, `-`.

The implementation includes two tiny fixed cache slots:
- Last-access cache for immediate repeats.
- Auto-promoted hot-entry cache for commonly used keys even under interleaved access.
