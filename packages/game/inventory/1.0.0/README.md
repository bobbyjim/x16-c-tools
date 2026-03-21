# Inventory Module

This directory implements a compact slot-based inventory, not a FIFO queue.

## Design Note

This layout is a good fit for memory-constrained systems:

- One slot costs exactly 1 byte.
- A full 32-slot inventory uses only 32 bytes.
- Common operations are branch-light and use simple bit masks/shifts.
- Empty detection is cheap (`cell == 0`).

Trade-offs:

- Max 8 item types (`0..7`) per slot encoding.
- Max per-slot quantity is 31.
- No metadata per slot beyond type/quantity unless you add side tables.

Each slot is a single byte packed as:

- `type` in the upper 3 bits (values `0..7`)
- `qty` in the lower 5 bits (values `1..31` for non-empty slots)
- `0x00` means an empty slot

Bit layout:

```text
bit:  7 6 5 4 3 2 1 0
      [ type ] [ qty ]
```

Equivalent expression:

```c
cell = (type << 5) | qty;
```

## Worked Examples

```text
raw=0x00 (00000000) -> type=0 qty=0   (empty slot)
raw=0x67 (01100111) -> type=3 qty=7
raw=0xBF (10111111) -> type=5 qty=31
raw=0xE1 (11100001) -> type=7 qty=1
```

Example encode/decode:

```c
uint8_t cell = inv_make(3, 7);   // 0x67
uint8_t t = inv_type(cell);      // 3
uint8_t q = inv_qty(cell);       // 7
```

## Key Constants

From `include/inventory.h`:

- `INV_TYPE_MASK = 0xE0`
- `INV_QTY_MASK  = 0x1F`
- `INV_MAX_TYPE  = 7`
- `INV_MAX_QTY   = 31`
- `INV_MIN_SIZE  = 4`
- `INV_MAX_SIZE  = 32`

## API Summary

Public API (declared in `include/inventory.h`):

- `uint8_t inv_make(uint8_t type, uint8_t qty)`
  - Packs type + quantity into one byte.
- `uint8_t inv_type(uint8_t cell)`
  - Extracts the 3-bit type.
- `uint8_t inv_qty(uint8_t cell)`
  - Extracts the 5-bit quantity.
- `inv_status inv_add_to_slot(...)`
  - Adds quantity into one slot.
  - If empty slot: stores `(type, qty)`.
  - If occupied slot with same type: stacks quantity.
  - If occupied slot with different type: returns `INV_ERR_TYPE`.
  - If stacking would exceed 31: returns `INV_ERR_OVERFLOW`.
- `inv_status inv_remove_from_slot(...)`
  - Returns slot byte and clears slot to `0`.
- `inv_status inv_clear(...)`
  - Clears all slots to `0`.

## Validation Rules

- Inventory pointer must be non-null.
- Size must be in `4..32`.
- Index must be within range.
- Type must be in `0..7`.
- Quantity passed to add must be in `1..31`.

## Status Codes

- `INV_OK = 0`
- `INV_ERR_NULL = -1`
- `INV_ERR_SIZE = -2`
- `INV_ERR_INDEX = -3`
- `INV_ERR_TYPE = -4`
- `INV_ERR_QTY = -5`
- `INV_ERR_OVERFLOW = -6`

## Build and Run

From this directory:

```sh
make
./tests
./cli 8
```

CLI commands:

- `add <index> <type> <qty>`
- `remove <index>`
- `print`
- `clear`
- `quit`
