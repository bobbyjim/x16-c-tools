#ifndef _DISK_HASH_H
#define _DISK_HASH_H

#define HASH_KEY_MAX_LEN 15
#define HASH_VALUE_MAX_LEN 39

int getSymbol( char* key, char* value );
int putSymbol( char* key, char* value );
void showDir();

#endif
