#include <stdio.h>
#include <stdint.h>

#include "auth.h"

static int tests_run = 0;
static int tests_failed = 0;

static void fail(const char *label, int line)
{
    printf("FAIL: %s (line %d)\n", label, line);
    tests_failed++;
}

#define EXPECT_RESULT(label, got, expected) do { \
    tests_run++; \
    if ((got) != (expected)) { \
        fail(label, __LINE__); \
        printf("  expected %d got %d\n", (int)(expected), (int)(got)); \
    } \
} while (0)

static void test_correct_code(void)
{
    EXPECT_RESULT("correct code", auth_check(123456, "123456"), AUTH_OK);
    EXPECT_RESULT("correct code zero", auth_check(0, "000000"), AUTH_OK);
    EXPECT_RESULT("correct code max", auth_check(999999, "999999"), AUTH_OK);
}

static void test_wrong_code(void)
{
    EXPECT_RESULT("wrong code off by one", auth_check(123456, "123457"), AUTH_FAIL);
    EXPECT_RESULT("wrong code all zeros", auth_check(1, "000000"), AUTH_FAIL);
}

static void test_null_input(void)
{
    EXPECT_RESULT("null input", auth_check(0, NULL), AUTH_FAIL);
}

static void test_too_short(void)
{
    EXPECT_RESULT("too short empty", auth_check(0, ""), AUTH_FAIL);
    EXPECT_RESULT("too short 5 digits", auth_check(12345, "12345"), AUTH_FAIL);
}

static void test_too_long(void)
{
    EXPECT_RESULT("too long 7 digits", auth_check(1234567, "1234567"), AUTH_FAIL);
    EXPECT_RESULT("too long with match prefix", auth_check(123456, "1234567"), AUTH_FAIL);
}

static void test_non_digit_chars(void)
{
    EXPECT_RESULT("letters", auth_check(0, "abcdef"), AUTH_FAIL);
    EXPECT_RESULT("mixed", auth_check(123456, "12345a"), AUTH_FAIL);
    EXPECT_RESULT("space", auth_check(0, "00 000"), AUTH_FAIL);
    EXPECT_RESULT("newline in input", auth_check(0, "00000\n"), AUTH_FAIL);
}

static void test_leading_zeros(void)
{
    EXPECT_RESULT("code with leading zeros", auth_check(42, "000042"), AUTH_OK);
    EXPECT_RESULT("code zero all zeros", auth_check(0, "000000"), AUTH_OK);
}

int main(void)
{
    test_correct_code();
    test_wrong_code();
    test_null_input();
    test_too_short();
    test_too_long();
    test_non_digit_chars();
    test_leading_zeros();

    printf("\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed > 0 ? 1 : 0;
}
