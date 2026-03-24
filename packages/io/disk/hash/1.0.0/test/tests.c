#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../include/hash.h"

static int tests_run = 0;
static int tests_failed = 0;

static void fail(const char* label, int line)
{
    printf("FAIL: %s (line %d)\n", label, line);
    tests_failed++;
}

#define EXPECT_TRUE(label, expr) do { \
    tests_run++; \
    if (!(expr)) { \
        fail(label, __LINE__); \
    } \
} while (0)

#define EXPECT_STREQ(label, got, expected) do { \
    tests_run++; \
    if (strcmp((got), (expected)) != 0) { \
        fail(label, __LINE__); \
        printf("  expected '%s' got '%s'\n", (expected), (got)); \
    } \
} while (0)

static void must_chdir(const char* path)
{
    if (chdir(path) != 0) {
        perror("chdir");
        exit(2);
    }
}

static void test_put_and_get_basic(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };

    EXPECT_TRUE("put alpha", putSymbol("alpha", "value-1") == 1);
    EXPECT_TRUE("get alpha", getSymbol("alpha", out) == 1);
    EXPECT_STREQ("alpha value", out, "value-1");
}

static void test_overwrite_value(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };

    EXPECT_TRUE("put overwrite 1", putSymbol("beta", "old") == 1);
    EXPECT_TRUE("put overwrite 2", putSymbol("beta", "new") == 1);
    EXPECT_TRUE("get overwrite", getSymbol("beta", out) == 1);
    EXPECT_STREQ("overwrite result", out, "new");
}

static void test_missing_key(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };

    EXPECT_TRUE("missing key", getSymbol("no_such_key", out) == 0);
}

static void test_invalid_keys(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };
    char long_key[HASH_KEY_MAX_LEN + 4];
    memset(long_key, 'a', sizeof(long_key) - 1);
    long_key[sizeof(long_key) - 1] = '\0';

    EXPECT_TRUE("reject empty key", putSymbol("", "x") == 0);
    EXPECT_TRUE("reject slash in key", putSymbol("bad/key", "x") == 0);
    EXPECT_TRUE("reject long key", putSymbol(long_key, "x") == 0);

    EXPECT_TRUE("reject invalid get key", getSymbol("../oops", out) == 0);
}

static void test_value_length_limit(void)
{
    char too_long[HASH_VALUE_MAX_LEN + 2];
    memset(too_long, 'v', sizeof(too_long) - 1);
    too_long[sizeof(too_long) - 1] = '\0';

    EXPECT_TRUE("reject long value", putSymbol("gamma", too_long) == 0);
}

static void test_cache_hit_after_file_removed(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };

    EXPECT_TRUE("put cache key", putSymbol("cachek", "cached") == 1);
    EXPECT_TRUE("remove backing file", unlink("hash-cachek.txt") == 0);
    EXPECT_TRUE("get cache key", getSymbol("cachek", out) == 1);
    EXPECT_STREQ("cache value", out, "cached");
}

static void test_hot_cache_survives_interleaving(void)
{
    char out[HASH_VALUE_MAX_LEN + 1] = { 0 };

    EXPECT_TRUE("put hot", putSymbol("hot", "hotv") == 1);
    EXPECT_TRUE("put cold", putSymbol("cold", "coldv") == 1);

    EXPECT_TRUE("warm hot 1", getSymbol("hot", out) == 1);
    EXPECT_TRUE("warm hot 2", getSymbol("hot", out) == 1);
    EXPECT_TRUE("interleave cold 1", getSymbol("cold", out) == 1);
    EXPECT_TRUE("warm hot 3", getSymbol("hot", out) == 1);
    EXPECT_TRUE("warm hot 4", getSymbol("hot", out) == 1);
    EXPECT_TRUE("interleave cold 2", getSymbol("cold", out) == 1);

    EXPECT_TRUE("remove hot backing file", unlink("hash-hot.txt") == 0);
    EXPECT_TRUE("touch cold after remove", getSymbol("cold", out) == 1);

    EXPECT_TRUE("hot from hot-cache", getSymbol("hot", out) == 1);
    EXPECT_STREQ("hot cache value", out, "hotv");
}

int main(void)
{
    char tmp[] = "/tmp/hash-tests-XXXXXX";
    char* test_dir = mkdtemp(tmp);
    if (!test_dir) {
        perror("mkdtemp");
        return 2;
    }

    must_chdir(test_dir);

    test_put_and_get_basic();
    test_overwrite_value();
    test_missing_key();
    test_invalid_keys();
    test_value_length_limit();
    test_cache_hit_after_file_removed();
    test_hot_cache_survives_interleaving();

    if (tests_failed == 0) {
        printf("ALL TESTS PASSED (%d)\n", tests_run);
        return 0;
    }

    printf("TESTS FAILED (%d of %d)\n", tests_failed, tests_run);
    return 1;
}
