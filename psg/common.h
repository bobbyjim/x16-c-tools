#ifndef _ADSR_TEST_H_
#define _ADSR_TEST_H_

#define	ABI				((unsigned char *)0x02)
#define ADSR_DATA		((unsigned char *)0x0412)

#define	ADDR_RESET_ENV		0x0400
#define ADDR_RUN_ENV		0x0403
#define ADDR_PLAYER			0x0406
#define ADDR_SAVE_ABI		0x0409
#define ADDR_LOAD_ABI		0x040c
#define ADDR_UPDATE_ABI		0x040f
// #define ADDR_INSTALLER		0x0412

typedef struct 
{
	unsigned state        : 8; // $02
	unsigned vol_lo       : 8; // $03 
	unsigned vol_hi       : 8; // $04
	unsigned sus_timer_lo : 8; // $05 
	unsigned sus_timer_hi : 8; // $06 
	unsigned att_lo       : 8; // $07 
	unsigned att_hi       : 8; // $08 
	unsigned dec_lo       : 8; // $09 
	unsigned dec_hi       : 8; // $0a 
	unsigned sus_level    : 8; // $0b 
	unsigned sus_lo       : 8; // $0c 
	unsigned sus_hi       : 8; // $0d 
	unsigned rls_lo       : 8; // $0e 
	unsigned rls_hi       : 8; // $0f 
	unsigned voice        : 8; // $10 

} AdsrFields;

typedef union
{
	AdsrFields env;
	unsigned char abi[15];
} ADSR;

void dumpADSRmemory();
void dumpABI();

void clearStruct( ADSR *adsr );
void clearABI();
void setABI( ADSR *adsr );
unsigned char compareABIto( unsigned char *values );
unsigned char compareDATAto( unsigned char *values, unsigned char voicenum );
#endif
