#include <stdint.h>

#include "inventory.h"

static inv_status inv_validate_size(uint8_t size)
{
    if (size < INV_MIN_SIZE || size > INV_MAX_SIZE) {
        return INV_ERR_SIZE;
    }
    return INV_OK;
}

static inv_status inv_validate_index(uint8_t size, uint8_t index)
{
    if (index >= size) {
        return INV_ERR_INDEX;
    }
    return INV_OK;
}

static inv_status inv_validate_type(uint8_t type)
{
    if (type > INV_MAX_TYPE) {
        return INV_ERR_TYPE;
    }
    return INV_OK;
}

static inv_status inv_validate_qty(uint8_t qty)
{
    if (qty == 0 || qty > INV_MAX_QTY) {
        return INV_ERR_QTY;
    }
    return INV_OK;
}

uint8_t inv_make(uint8_t type, uint8_t qty)
{
    return (uint8_t)((type << 5) | (qty & INV_QTY_MASK));
}

uint8_t inv_type(uint8_t cell)
{
    return (uint8_t)((cell & INV_TYPE_MASK) >> 5);
}

uint8_t inv_qty(uint8_t cell)
{
    return (uint8_t)(cell & INV_QTY_MASK);
}

inv_status inv_add_to_slot(uint8_t *inv, uint8_t size, uint8_t index, uint8_t type, uint8_t qty, uint8_t *out_newbyte)
{
    inv_status status;
    uint8_t current;
    uint8_t curr_qty;
    uint8_t curr_type;
    uint8_t new_qty;
    uint8_t new_byte;

    if (inv == 0) {
        return INV_ERR_NULL;
    }

    status = inv_validate_size(size);
    if (status != INV_OK) {
        return status;
    }

    status = inv_validate_index(size, index);
    if (status != INV_OK) {
        return status;
    }

    status = inv_validate_type(type);
    if (status != INV_OK) {
        return status;
    }

    status = inv_validate_qty(qty);
    if (status != INV_OK) {
        return status;
    }

    current = inv[index];
    curr_qty = inv_qty(current);
    curr_type = inv_type(current);

    if (curr_qty == 0) {
        new_qty = qty;
        new_byte = inv_make(type, new_qty);
    } else {
        if (curr_type != type) {
            return INV_ERR_TYPE;
        }
        if ((uint8_t)(curr_qty + qty) > INV_MAX_QTY) {
            return INV_ERR_OVERFLOW;
        }
        new_qty = (uint8_t)(curr_qty + qty);
        new_byte = inv_make(type, new_qty);
    }

    inv[index] = new_byte;

    if (out_newbyte != 0) {
        *out_newbyte = new_byte;
    }

    return INV_OK;
}

inv_status inv_remove_from_slot(uint8_t *inv, uint8_t size, uint8_t index, uint8_t *out_removed)
{
    inv_status status;
    uint8_t removed;

    if (inv == 0) {
        return INV_ERR_NULL;
    }

    status = inv_validate_size(size);
    if (status != INV_OK) {
        return status;
    }

    status = inv_validate_index(size, index);
    if (status != INV_OK) {
        return status;
    }

    removed = inv[index];
    inv[index] = 0;

    if (out_removed != 0) {
        *out_removed = removed;
    }

    return INV_OK;
}

inv_status inv_clear(uint8_t *inv, uint8_t size)
{
    inv_status status;
    uint8_t i;

    if (inv == 0) {
        return INV_ERR_NULL;
    }

    status = inv_validate_size(size);
    if (status != INV_OK) {
        return status;
    }

    for (i = 0; i < size; ++i) {
        inv[i] = 0;
    }

    return INV_OK;
}
