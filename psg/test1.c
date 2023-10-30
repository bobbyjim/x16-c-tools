#include <conio.h>
#include <6502.h>

#include "common.h"

unsigned char test11_ABI()
{
	ADSR adsr;
	unsigned char correct[15] = { 3, 7, 2, 15, 12, 11, 1, 8, 6, 4, 10, 5, 9, 13, 14 };

	//cputs("test 1: populate ABI\r\n");

	adsr.env.state        = correct[0];
	adsr.env.vol_lo		  = correct[1];
	adsr.env.vol_hi       = correct[2];
	adsr.env.sus_timer_lo = correct[3];
	adsr.env.sus_timer_hi = correct[4];
	adsr.env.att_lo		  = correct[5];
	adsr.env.att_hi       = correct[6];
	adsr.env.dec_lo		  = correct[7];
	adsr.env.dec_hi       = correct[8];
	adsr.env.sus_level    = correct[9];
	adsr.env.sus_lo 	  = correct[10];
	adsr.env.sus_hi       = correct[11];
	adsr.env.rls_lo		  = correct[12];
	adsr.env.rls_hi       = correct[13];
	adsr.env.voice        = correct[14];

    //clearABI();
	setABI( &adsr );

	return compareABIto( correct );
}

unsigned char test12_ABI_to_memory()
{
	unsigned char i = 15;
	unsigned char correct[15] = { 3, 7, 2, 15, 12, 11, 1, 8, 6, 4, 10, 5, 9, 13, 3 };
	struct regs r;

	while(i--) { ABI[i] = correct[i]; }

	r.pc = ADDR_SAVE_ABI;
	r.flags = 0;
	_sys(&r);

    //dumpADSRmemory();

	return compareDATAto( correct, 3 );
}

unsigned char test13_memory_to_ABI()
{
	unsigned char correct[15] = { 3, 7, 2, 15, 12, 11, 1, 8, 6, 4, 10, 5, 9, 13, 3 };
	struct regs r;

	clearABI();
	r.pc = ADDR_LOAD_ABI;
	r.x  = 3; // voice number
	r.flags = 0;
	_sys(&r);

	return compareABIto( correct );
}

unsigned char test14_ResetState()
{
	struct regs r;

	ABI[0] = 45; // or whatever

	// $0400 sets 0 to ENV_STATE ($02)
	r.pc = ADDR_RESET_ENV;
	r.flags = 0;
	
    //dumpABI();
	_sys(&r);
	//dumpABI();

	return ABI[0] == 0; // should be reset, right??
}

