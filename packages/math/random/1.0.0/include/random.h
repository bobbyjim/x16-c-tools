#ifndef __BURTLE2__
#define __BURTLE2__

typedef unsigned long int word;
typedef struct ranctx2 { word a; word b; word c; word d; } ranctx2;

void srand(word seed);
word rand();

#endif
