/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/sys.h"
	#include	"../library/vfs.h"
	// external ------------------------------------------------------------
	#include	"console.h"
	//======================================================================

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve current uptime
	uint64_t seconds = lib_sys_uptime() / 1000;	// 1000ms per second

	// days
	uint64_t d = seconds / 86400;
	if( d ) { printf( STATIC_SEQUENCE_COLOR_GREEN_LIGHT"%d"STATIC_SEQUENCE_COLOR_DEFAULT" day", d ); if( d > 1 ) print( "s" ); print( ", " ); }

	// truncrate seconds
	seconds %= 86400;

	// hours
	uint8_t h = seconds / 3600;
	if( h < STATIC_NUMBER_SYSTEM_decimal ) print( "0" );
	printf( "%d", h );

	// separator
	print( ":" );

	// truncrate seconds
	seconds %= 3600;

	// minutes
	uint8_t m = seconds / 60;
	if( m < STATIC_NUMBER_SYSTEM_decimal ) print( "0" );
	printf( "%d", m );

	// separator
	print( ":" );

	// truncrate seconds
	seconds %= 60;

	// seconds
	if( seconds < STATIC_NUMBER_SYSTEM_decimal ) print( "0" );
	printf( "%d", seconds );

	// end of execution
	return 0;
}
