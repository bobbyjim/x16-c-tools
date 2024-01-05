#include <conio.h>
#include <6502.h>

#include "common.h"
#include "test4.h"

//
//  Test release logic
//
unsigned char test41_Rel1()
{
	ADSR adsr;
	struct regs r;

	clearStruct(&adsr);
	adsr.env.state     = 8; 
	adsr.env.vol_lo    = 0;
	adsr.env.vol_hi    = 5;
	adsr.env.rls_lo    = 0;
	adsr.env.rls_hi    = 2;

	setABI( &adsr );

	r.pc =  ADDR_RUN_ENV; // RELEASE;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI_STATE == 8) && (ABI_VOL_LO == 0) && (ABI_VOL_HI == 3);
}

unsigned char test41_Rel2()
{
	ADSR adsr;
	struct regs r;

	clearStruct(&adsr);
	adsr.env.state     = 8; 
	adsr.env.vol_lo    = 0;
	adsr.env.vol_hi    = 5;
	adsr.env.rls_lo    = 3;
	adsr.env.rls_hi    = 0;

	setABI( &adsr );

	r.pc =  ADDR_RUN_ENV; // RELEASE;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI_STATE == 8) && (ABI_VOL_LO == 253) && (ABI_VOL_HI == 4);
}
unsigned char test41_Rel3()
{
	ADSR adsr;
	struct regs r;
	clearStruct(&adsr);
	adsr.env.state     = 8; 
	adsr.env.vol_lo    = 16;
	adsr.env.vol_hi    = 9;
	adsr.env.rls_lo    = 4;
	adsr.env.rls_hi    = 10;
	setABI( &adsr );

	r.pc =  ADDR_RUN_ENV; // RELEASE;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return ABI_STATE == 0;
}
