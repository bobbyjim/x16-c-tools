#ifndef _INVENTORY_H_
#define _INVENTORY_H_

#include <stdint.h>

#define INV_TYPE_MASK 0xE0
#define INV_QTY_MASK  0x1F

#define INV_MAX_QTY   31
#define INV_MAX_TYPE  7

#define INV_MIN_SIZE  4
#define INV_MAX_SIZE  32

typedef enum {
    INV_OK = 0,
    INV_ERR_NULL = -1,
    INV_ERR_SIZE = -2,
    INV_ERR_INDEX = -3,
    INV_ERR_TYPE = -4,
    INV_ERR_QTY = -5,
    INV_ERR_OVERFLOW = -6
} inv_status;

uint8_t inv_make(uint8_t type, uint8_t qty);
uint8_t inv_type(uint8_t cell);
uint8_t inv_qty(uint8_t cell);

inv_status inv_add_to_slot(uint8_t *inv, uint8_t size, uint8_t index, uint8_t type, uint8_t qty, uint8_t *out_newbyte);
inv_status inv_remove_from_slot(uint8_t *inv, uint8_t size, uint8_t index, uint8_t *out_removed);
inv_status inv_clear(uint8_t *inv, uint8_t size);

#endif
