#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "inventory.h"

static void print_cell(uint8_t index, uint8_t cell)
{
    printf("[%u] type=%u qty=%u raw=0x%02X\n", index, inv_type(cell), inv_qty(cell), cell);
}

static void print_inventory(uint8_t *inv, uint8_t size)
{
    uint8_t i;
    for (i = 0; i < size; ++i) {
        print_cell(i, inv[i]);
    }
}

static void print_status(inv_status status)
{
    if (status == INV_OK) {
        printf("OK\n");
        return;
    }
    printf("ERROR %d\n", (int)status);
}

static void print_help(void)
{
    printf("Commands:\n");
    printf("  add <index> <type> <qty>\n");
    printf("  remove <index>\n");
    printf("  print\n");
    printf("  clear\n");
    printf("  quit\n");
}

int main(int argc, char **argv)
{
    uint8_t size;
    uint8_t inv[INV_MAX_SIZE];
    char line[128];

    if (argc < 2) {
        printf("Usage: %s <size 4-32>\n", argv[0]);
        return 1;
    }

    size = (uint8_t)atoi(argv[1]);
    if (inv_clear(inv, size) != INV_OK) {
        printf("Invalid size: %u\n", size);
        return 1;
    }

    print_help();

    for (;;) {
        char *cmd;
        char *tok;

        printf("> ");
        if (fgets(line, sizeof(line), stdin) == 0) {
            break;
        }

        cmd = strtok(line, " \t\r\n");
        if (cmd == 0) {
            continue;
        }

        if (strcmp(cmd, "add") == 0) {
            uint8_t index;
            uint8_t type;
            uint8_t qty;
            inv_status status;

            tok = strtok(0, " \t\r\n");
            if (tok == 0) { print_help(); continue; }
            index = (uint8_t)atoi(tok);

            tok = strtok(0, " \t\r\n");
            if (tok == 0) { print_help(); continue; }
            type = (uint8_t)atoi(tok);

            tok = strtok(0, " \t\r\n");
            if (tok == 0) { print_help(); continue; }
            qty = (uint8_t)atoi(tok);

            status = inv_add_to_slot(inv, size, index, type, qty, 0);
            print_status(status);
            continue;
        }

        if (strcmp(cmd, "remove") == 0) {
            uint8_t index;
            uint8_t removed = 0;
            inv_status status;

            tok = strtok(0, " \t\r\n");
            if (tok == 0) { print_help(); continue; }
            index = (uint8_t)atoi(tok);

            status = inv_remove_from_slot(inv, size, index, &removed);
            print_status(status);
            if (status == INV_OK) {
                print_cell(index, removed);
            }
            continue;
        }

        if (strcmp(cmd, "print") == 0) {
            print_inventory(inv, size);
            continue;
        }

        if (strcmp(cmd, "clear") == 0) {
            print_status(inv_clear(inv, size));
            continue;
        }

        if (strcmp(cmd, "quit") == 0) {
            break;
        }

        print_help();
    }

    return 0;
}
