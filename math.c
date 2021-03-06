#include <stdlib.h>

#include "math.h"

/* static will allow inlining */
/*static*/ unsigned usqrt4(unsigned val) {
    unsigned a, b;

    if (val < 2) return val; /* avoid div/0 */

    a = 1255;       /* starting point is relatively unimportant */

    b = val / a; a = (a+b) /2;
    b = val / a; a = (a+b) /2;
    b = val / a; a = (a+b) /2;
    b = val / a; a = (a+b) /2;

    return a;
}

long square(int val)
{
    return val * val;
}

long signedSquare(int val)
{
    return val * abs(val);
}

long cube(int val)
{
    return val * val * val;
}

