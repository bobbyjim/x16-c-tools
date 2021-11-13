//
//  This variant is based on:
//  https://www.pcg-random.org/posts/bob-jenkins-small-prng-passes-practrand.html#
//
//  cl65 -O -t cx16 burtle.c
//
#include <stdio.h>
#include "util.h"
#include "burtle2.h"

ranctx2 burtle;

#define rot(x,k)    (((x)<<(k))|((x)>>(16-(k))))

word ranval( ranctx2 *x )
{
   word e = x->a - rot(x->b, 13);
   x->a = x->b ^ rot(x->c, 9); // ..or 8?
   x->b = x->c + x->d;
   x->c = x->d + e;
   x->d = e + x->a;
   return x->d;
}

void raninit( ranctx2 *x, word seed )
{
   word i;
   x->a = 0x5eed, x->b = x->c = x->d = seed;
   for (i=0; i<20; ++i) {
      (void)ranval(x);
   }
}

void srand(word seed) 
{
   raninit(&burtle,seed);
}

word rand() 
{ 
   return ranval(&burtle); 
}

/*
int main(void)
{
   word seed = 5;
   int i=0;

   srand(seed);

   for(i=0; i<100000; ++i)
      printf( "%u\n", rand() );

   return(0);
}
*/

