#include <stdio.h>
#include <stdlib.h>
#include <cbm.h>

typedef unsigned long int u4;
typedef struct ranctx { u4 a; u4 b; u4 c; u4 d; } ranctx;

#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))

u4 ranval( ranctx *x ) 
{
   u4 e = x->a - rot(x->b, 27);
   x->a = x->b ^ rot(x->c, 17);
   x->b = x->c + x->d;
   x->c = x->d + e;
   x->d = e + x->a;
   return x->d;
}

void raninit( ranctx *x, u4 seed ) 
{
   u4 i;
   x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
   for (i=0; i<20; ++i) {
      (void)ranval(x);
   }
}

