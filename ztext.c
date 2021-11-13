#include <stdio.h>
#include <string.h>
#include <peekpoke.h>

#include "ztext.h"

char *alphabet[2] = { " abcdefghijklmnopqrstuvwxyz?:'-", " 0123456789abcdefghilmnorstuwy-" };
unsigned char zcase = 0;
//char outbuf[256];
unsigned char outbufpos = 0;

unsigned char b0, b1, b2, b3, b4, b5;

void decodeZbyte(unsigned char b, char* outbuf) 
{
   if (b == 31) 
   {
      zcase = 1 - zcase;
   }
   else
   {
      outbuf[outbufpos] = alphabet[zcase][b];
      printf( "%d: %c\n", outbufpos, outbuf[outbufpos] );
      ++outbufpos;
   }
}

char* decodeZtext(unsigned startLoc, char* outbuf)
{
   b5 = 0;
   outbufpos = 0;

   for(; b5 == 0; startLoc += 2)
   {
      b0 = PEEK(startLoc);
      b1 = PEEK(startLoc+1);

      b2 = b0 & 31;
      b3 = (b0/32) + (b1 & 3)*8;
      b4 = (b1 & 127)/4;
      b5 = (b1/128);
  
      decodeZbyte(b2, outbuf);    
      decodeZbyte(b3, outbuf);    
      decodeZbyte(b4, outbuf);    
   }

   outbuf[outbufpos] = 0; // terminate string
   return outbuf;
}

char* decodeZtextbuf(unsigned char *buf, char* outbuf)
{
   unsigned char loc;
   outbufpos = 0;

   for(loc=0; loc<strlen(buf); loc += 2)
   {
      b0 = buf[loc];
      b1 = buf[loc+1];

      b2 = b0 & 31;
      b3 = (b0/32) + (b1 & 3)*8;
      b4 = (b1 & 127)/4;
      b5 = (b1/128);

      decodeZbyte(b2, outbuf);
      decodeZbyte(b3, outbuf);
      decodeZbyte(b4, outbuf);

      if (b5 == 1) break;
   }

   outbuf[outbufpos] = 0; // terminate string
   return outbuf;
}

/*
void main()
{
   char buf1[8] = { 162, 12, 171, 79, 224, 62, 146, 145 };
   char buf2[8] = { 137, 34, 79,  58, 46,  76, 244, 201 };
   char outbuf[256];

   decodeZtextbuf( buf1, outbuf );
   printf( "%s\n", outbuf );

   decodeZtextbuf( buf2, outbuf );
   printf( "%s\n", outbuf );
}
*/

