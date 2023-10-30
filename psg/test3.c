#include <conio.h>
#include <6502.h>

#include "common.h"
#include "test3.h"

//
//  Just add to vol_lo and vol_hi
//
unsigned char test31_Attack1()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	// setting ATT to some value
	// setting VOL to some value
	// setting the STATE to "2" (I think)
	// calling env_run ($0403)
	// verifying that VOL has changed approriately

	adsr.env.state    = 2;
	adsr.env.att_lo   = 2;
	adsr.env.att_hi   = 3;
	adsr.env.vol_lo   = 4;
	adsr.env.vol_hi   = 4;

	setABI( &adsr );

	r.pc = ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 2) && (ABI[1] == 6) && (ABI[2] == 7);
}

//
//  16 bit adder should work
//
unsigned char test31_Attack2()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	// setting ATT to some value
	// setting VOL to some value
	// setting the STATE to "2" (I think)
	// calling env_run ($0403)
	// verifying that VOL has changed approriately

	adsr.env.state    = 2;
	adsr.env.att_lo   = 2;
	adsr.env.att_hi   = 0;
	adsr.env.vol_lo   = 255;
	adsr.env.vol_hi   = 4;

	setABI( &adsr );

	r.pc = ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 2) && (ABI[1] == 1) && (ABI[2] == 5);
}

//
//  should not overflow
//
unsigned char test31_Attack3()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	// setting ATT to some value
	// setting VOL to some value
	// setting the STATE to "2" (I think)
	// calling env_run ($0403)
	// verifying that VOL has changed approriately

	adsr.env.state    = 2;
	adsr.env.att_lo   = 2;
	adsr.env.att_hi   = 3;
	adsr.env.vol_lo   = 0;
	adsr.env.vol_hi   = 62;

	setABI( &adsr );

	r.pc = ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 4) && (ABI[1] == 2) && (ABI[2] == 63);
}

//
//  Normal easy decay
//
unsigned char test32_Decay1()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	adsr.env.state     = 4; 
	adsr.env.vol_lo    = 5;
	adsr.env.vol_hi    = 4;
	adsr.env.dec_lo    = 1;
	adsr.env.dec_hi    = 2;
	adsr.env.sus_level = 0;

	setABI( &adsr );

	r.pc = ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 4) && (ABI[1] == 4) && (ABI[2] == 2);
}

//
//  16 bit decay should world
//
unsigned char test32_Decay2()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	adsr.env.state     = 4; 
	adsr.env.vol_lo    = 5;
	adsr.env.vol_hi    = 4;
	adsr.env.dec_lo    = 10;
	adsr.env.dec_hi    = 0;
	adsr.env.sus_level = 0;

	setABI( &adsr );

	r.pc = ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 4) && (ABI[1] == 251) && (ABI[2] == 3);
}

//
//  state transition
//
unsigned char test32_Decay3()
{
	ADSR adsr;
	struct regs r;

	unsigned char initialization[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char i = 15;
	while(i--) adsr.abi[i] = 0;

	adsr.env.state     = 4; 
	adsr.env.vol_lo    = 0;
	adsr.env.vol_hi    = 5;
	adsr.env.dec_lo    = 0;
	adsr.env.dec_hi    = 2;
	adsr.env.sus_level = 5;
	adsr.env.sus_lo    = 0;
	adsr.env.sus_hi    = 1;

	setABI( &adsr );

	r.pc =  0x62f;  // (decay)  was: ADDR_RUN_ENV;
	r.flags = 0;

    //dumpABI();
	_sys(&r);
	//dumpABI();

	return (ABI[0] == 6) && (ABI[1] == 0) && (ABI[2] == 5);
}

