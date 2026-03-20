#include <stdio.h>
#include <stdint.h>

#include "inventory.h"

static int tests_run = 0;
static int tests_failed = 0;

static void fail(const char *label, int line)
{
    printf("FAIL: %s (line %d)\n", label, line);
    tests_failed++;
}

#define EXPECT_EQ(label, a, b) do { \
    tests_run++; \
    if ((a) != (b)) { \
        fail(label, __LINE__); \
        printf("  expected %d got %d\n", (int)(b), (int)(a)); \
    } \
} while (0)

#define EXPECT_STATUS(label, status, expected) do { \
    tests_run++; \
    if ((status) != (expected)) { \
        fail(label, __LINE__); \
        printf("  expected %d got %d\n", (int)(expected), (int)(status)); \
    } \
} while (0)

static void test_make_and_extract(void)
{
    uint8_t cell = inv_make(3, 17);
    EXPECT_EQ("type extract", inv_type(cell), 3);
    EXPECT_EQ("qty extract", inv_qty(cell), 17);
}

static void test_add_empty(void)
{
    uint8_t inv[8] = { 0 };
    uint8_t out = 0;
    inv_status status = inv_add_to_slot(inv, 8, 2, 5, 7, &out);

    EXPECT_STATUS("add empty status", status, INV_OK);
    EXPECT_EQ("add empty qty", inv_qty(inv[2]), 7);
    EXPECT_EQ("add empty type", inv_type(inv[2]), 5);
    EXPECT_EQ("add empty out", out, inv[2]);
}

static void test_add_stack_ok(void)
{
    uint8_t inv[8] = { 0 };
    inv[1] = inv_make(2, 10);

    EXPECT_STATUS("stack ok", inv_add_to_slot(inv, 8, 1, 2, 5, 0), INV_OK);
    EXPECT_EQ("stack qty", inv_qty(inv[1]), 15);
}

static void test_add_stack_overflow(void)
{
    uint8_t inv[8] = { 0 };
    inv[0] = inv_make(1, 30);

    EXPECT_STATUS("stack overflow", inv_add_to_slot(inv, 8, 0, 1, 2, 0), INV_ERR_OVERFLOW);
    EXPECT_EQ("overflow unchanged", inv_qty(inv[0]), 30);
}

static void test_add_wrong_type(void)
{
    uint8_t inv[8] = { 0 };
    inv[3] = inv_make(1, 4);

    EXPECT_STATUS("wrong type", inv_add_to_slot(inv, 8, 3, 2, 1, 0), INV_ERR_TYPE);
    EXPECT_EQ("wrong type unchanged", inv[3], inv_make(1, 4));
}

static void test_remove(void)
{
    uint8_t inv[8] = { 0 };
    uint8_t removed = 0;

    inv[4] = inv_make(7, 31);
    EXPECT_STATUS("remove status", inv_remove_from_slot(inv, 8, 4, &removed), INV_OK);
    EXPECT_EQ("remove byte", removed, inv_make(7, 31));
    EXPECT_EQ("remove cleared", inv[4], 0);
}

static void test_size_validation(void)
{
    uint8_t inv[4] = { 0 };

    EXPECT_STATUS("size too small", inv_add_to_slot(inv, 3, 0, 1, 1, 0), INV_ERR_SIZE);
    EXPECT_STATUS("size too big", inv_add_to_slot(inv, 33, 0, 1, 1, 0), INV_ERR_SIZE);
}

int main(void)
{
    test_make_and_extract();
    test_add_empty();
    test_add_stack_ok();
    test_add_stack_overflow();
    test_add_wrong_type();
    test_remove();
    test_size_validation();

    if (tests_failed == 0) {
        printf("ALL TESTS PASSED (%d)\n", tests_run);
        return 0;
    }

    printf("TESTS FAILED (%d of %d)\n", tests_failed, tests_run);
    return 1;
}
