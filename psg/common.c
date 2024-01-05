#include <conio.h>
#include <string.h>
#include "common.h"

void dumpADSRmemory()
{
    unsigned char index;

	for(index = 0; index < 224; ++index)
	{
		if (index % 16 == 0) cprintf("\r\n %04x: ", ADSR_DATA + index);
		cprintf("%02x ", ADSR_DATA[index]);
	}
	cputs("\r\n");
}

void dumpABI()
{
	unsigned char index = 16;

    cputs("   " );
	while(index--)
	{
		gotox( 8 + index * 3 );
		cprintf( "%2x", ABI[index] );
	}
	cputs( "\r\n" );
}

char log[80];

void rememberABI()
{
//	unsigned char index = 16;

//    sprintf( target, "   " );
//	while(index--)
//	{
//		sprintf( target, "%2x ", ABI[index] );
//	}
//	cputs( "\r\n" );
}


void clearStruct( ADSR *adsr )
{
	int i = 15;
	while(i--) adsr->abi[i] = 0;
}

void clearABI() 
{
	unsigned char i = 15;
	while(i--) ABI[i] = 0;
}

void setABI( ADSR *adsr )
{
	int i = 15;
	while(i--) ABI[i] = adsr->abi[i];
}

unsigned char compareABIto( unsigned char *values )
{
	unsigned char i      = 14;
	unsigned char all_ok = 1;

	unsigned char ok;

	while(i--)
	{
		ok = 0;
		if (ABI[i] == values[i]) ok = 1;
		else all_ok = 0;
		if (!ok)
		   cprintf("  - %2u : %02x  ==  %02x  %s\r\n", i, ABI[i], values[i], ok? "" : "fail");
	}
	return all_ok;
}

unsigned char compareDATAto( unsigned char *values, unsigned char voice_num )
{
	unsigned char i      = 14; // not voice
	unsigned char all_ok = 1;

	unsigned char ok;

	while(i--)
	{
		ok = 0;
		if (ADSR_DATA[voice_num + i * 16] == values[i]) ok = 1;
		else all_ok = 0;
		if (!ok)
		   cprintf("  - %4x : %02x  ==  %02x  %s\r\n", ADSR_DATA + voice_num + i * 16, ADSR_DATA[voice_num + i * 16], values[i], ok? "" : "fail");
	}
	return all_ok;
}
