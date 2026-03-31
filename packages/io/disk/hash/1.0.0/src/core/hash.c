#include "../include/hash.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

/* Two tiny fixed cache slots: last-access plus auto-promoted hot key. */
static unsigned char last_cache_valid = 0;
static char last_cache_key[HASH_KEY_MAX_LEN + 1];
static char last_cache_value[HASH_VALUE_MAX_LEN + 1];

static unsigned char hot_cache_valid = 0;
static char hot_cache_key[HASH_KEY_MAX_LEN + 1];
static char hot_cache_value[HASH_VALUE_MAX_LEN + 1];

/* Lightweight frequency tracker (Space-Saving style) for hot-key promotion. */
static unsigned char freq_valid = 0;
static unsigned char freq_score = 0;
static char freq_key[HASH_KEY_MAX_LEN + 1];

#define HOT_PROMOTE_SCORE 3

static int key_is_valid(const char* key)
{
   size_t i;
   size_t len;

   if (!key) {
      return 0;
   }

   len = strlen(key);
   if (len == 0 || len > HASH_KEY_MAX_LEN) {
      return 0;
   }

   for (i = 0; i < len; ++i) {
      if (!(isalnum((unsigned char)key[i]) || key[i] == '_' || key[i] == '-')) {
         return 0;
      }
   }

   return 1;
}

static int value_is_valid(const char* value)
{
   if (!value) {
      return 0;
   }

   return strlen(value) <= HASH_VALUE_MAX_LEN;
}

static int build_filename(const char* key, char* out, size_t out_size)
{
   int n;

   n = snprintf(out, out_size, "hash-%s.txt", key);
   return (n > 0 && (size_t)n < out_size);
}

static void cache_last_set(const char* key, const char* value)
{
   strcpy(last_cache_key, key);
   strcpy(last_cache_value, value);
   last_cache_valid = 1;
}

static void cache_hot_set(const char* key, const char* value)
{
   strcpy(hot_cache_key, key);
   strcpy(hot_cache_value, value);
   hot_cache_valid = 1;
}

static void update_hot_candidate(const char* key)
{
   if (!freq_valid) {
      strcpy(freq_key, key);
      freq_score = 1;
      freq_valid = 1;
      return;
   }

   if (strcmp(freq_key, key) == 0) {
      if (freq_score < 255) {
         ++freq_score;
      }
      return;
   }

   if (freq_score > 0) {
      --freq_score;
   }

   if (freq_score == 0) {
      strcpy(freq_key, key);
      freq_score = 1;
   }
}

static void maybe_promote_hot(const char* key, const char* value)
{
   update_hot_candidate(key);
   if (freq_score >= HOT_PROMOTE_SCORE && strcmp(freq_key, key) == 0) {
      cache_hot_set(key, value);
   }
}

int getSymbol( char* key, char* value )
{
   char filename[32];
   FILE* fp;

   if (!value || !key_is_valid(key)) {
      return 0;
   }

   if (last_cache_valid && strcmp(key, last_cache_key) == 0) {
      strcpy(value, last_cache_value);
      maybe_promote_hot(key, value);
      return 1;
   }

   if (hot_cache_valid && strcmp(key, hot_cache_key) == 0) {
      strcpy(value, hot_cache_value);
      cache_last_set(key, value);
      maybe_promote_hot(key, value);
      return 1;
   }

   if (!build_filename(key, filename, sizeof(filename))) {
      return 0;
   }

   fp = fopen(filename, "r");
   if (!fp) {
      return 0;
   }

   if (!fgets(value, HASH_VALUE_MAX_LEN + 1, fp)) {
      fclose(fp);
      return 0;
   }
   fclose(fp);

   value[strcspn(value, "\r\n")] = '\0';

   cache_last_set(key, value);
   maybe_promote_hot(key, value);

   return 1;
}

int putSymbol( char* key, char* value )
{
   char filename[32];
   FILE* fp;

   if (!key_is_valid(key) || !value_is_valid(value)) {
      return 0;
   }

   if (!build_filename(key, filename, sizeof(filename))) {
      return 0;
   }

   fp = fopen(filename, "w");
   if (!fp) {
      return 0;
   }

   if (fputs(value, fp) < 0) {
      fclose(fp);
      return 0;
   }

   if (fclose(fp) != 0) {
      return 0;
   }

   cache_last_set(key, value);
   maybe_promote_hot(key, value);

   return 1;
}

void showDir()
{
   DIR *dir;
   struct dirent *ent;

   dir = opendir(".");
   if (!dir) {
      return;
   }

   while ((ent = readdir(dir)) != 0) {
      printf("%s\n", ent->d_name);
   }

   closedir(dir);
}
