/*
 *  Test harness for my assembly ADSR code.
 *  With this code I can set up sounds, adjust the envelope settings easily,
 *  and then see what happens.
 */
#include <conio.h>

#include "test1.h"
#include "test3.h"
#include "test4.h"
#include "test5.h"

unsigned char all_pass = 1;

void processResult( unsigned char ok, char *name )
{
	if (!ok) all_pass = 0;
	cprintf( "   %-4s : %s\r\n", ok? "ok" : "FAIL", name );
}

char abiBuf[80];

void main() 
{
	clrscr();
	cputs("\r\n      ADSR TEST SUITE\r\n\r\n");

	// test 1
	// jmp ssnd		  ; $0409
	// jmp lsnd		  ; $040c
	// jmp usnd		  ; $040f
	processResult( test11_ABI(), "test MEM 1 - populate ABI" );
	processResult( test12_ABI_to_memory(), "test MEM 2 - transfer ABI to ADSR memory" );
	processResult( test13_memory_to_ABI(), "test MEM 3 - transfer ADSR memory to ABI" );
	// jmp env_reset  ; $0400 sets 0 to ENV_STATE ($02)
	processResult( test14_ResetState(), "test MEM 4 - reset ABI state");

	// test attack, decay
	processResult( test31_Attack1(), "test ATT 1" );
	processResult( test31_Attack2(), "test ATT 2 - 16 bit add" );
	processResult( test31_Attack3(), "test ATT 3 - max vol state transition" );
	processResult( test32_Decay1(),   "test DEC 1" );
	processResult( test32_Decay2(),   "test DEC 2 - 16 bit sub" );
	processResult( test32_Decay3(),   "test DEC 3 - min vol state transition" );

	// check the trigger from ATT to SUS.

    // check RLS
	processResult( test41_Rel1(), "test RLS 1 - reduce vol hi");
	processResult( test41_Rel2(), "test RLS 2 - 16 bit sub");
	processResult( test41_Rel3(), "test RLS 3 - state transition");


	if (all_pass == 1)
	{
		processResult( test51_boom(), "test BOOM" );
	}
}
