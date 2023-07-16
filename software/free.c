/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/integer.h"
	#include	"../library/shared/sys.h"
	// external ------------------------------------------------------------
	#include	"console.h"
	//======================================================================

void unit( uint8_t unit ) {
	switch( unit ) {
		case 1:
			print( "KiB " );
			break;
		case 2:
			print( "MiB " );
			break;
		case 3:
			print( "GiB " );
			break;
		case 4:
			print( "TiB " );
			break;
	}
}

void degradation( uint64_t bytes ) {
	if( bytes >= 0x10000000000 ) { printf( "%4d ", bytes / 0x10000000000 ); unit( 4 ); return; }
	if( bytes >= 0x40000000 ) { printf( "%4d ", bytes / 0x40000000 ); unit( 3 ); return; }
	if( bytes >= 0x100000 ) { printf( "%4d ", bytes / 0x100000 ); unit( 2 ); return; }
	if( bytes >= 0x400 ) { printf( "%4d ", bytes / 0x400 ); unit( 1 ); return; }
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream propertries
	struct CONSOLE_STRUCTURE_META console_meta;
	while( lib_sys_stream_get( (uint8_t *) &console_meta, LIB_SYS_STREAM_out ) & LIB_SYS_STREAM_FLAG_undefinied );

	// retrieve current memory usage
	struct LIB_SYS_STRUCTURE_MEMORY memory;
	lib_sys_memory( (struct LIB_SYS_STRUCTURE_MEMORY *) &memory );

	// show header
	print( "            Total     Used     Free\n" );

	//----------------------------------------------------------------------
	// memory
	print( " Memory: " );

	// total
	degradation( memory.total << STATIC_PAGE_SIZE_shift );

	// used
	degradation( (memory.total - memory.available) << STATIC_PAGE_SIZE_shift );

	// free
	degradation( memory.available << STATIC_PAGE_SIZE_shift );

	// show status bar

	// width of status bar
	int16_t width = console_meta.width - (36 + 4);
	if( width < 10 ) return 0;	// no enough space to show anything

	// calculate usage in percents
	uint8_t used = (uint8_t) (((double) (memory.total - memory.available) / (double) memory.total) * (double) width);
	uint8_t free = ((double) memory.available / (double) memory.total) * (double) width;

	// by default status bar is green
	print( STATIC_SEQUENCE_COLOR_GREEN_LIGHT );

	// if there is less than half or quartes space, change color
	if( memory.available < memory.total >> STATIC_DIVIDE_BY_2_shift ) print( STATIC_SEQUENCE_COLOR_YELLOW );
	if( memory.available < memory.total >> STATIC_DIVIDE_BY_4_shift ) print( STATIC_SEQUENCE_COLOR_RED_LIGHT );

	// show usage
	while( used-- ) print( "|" );

	// and show left space in default color
	print( STATIC_SEQUENCE_COLOR_GRAY );
	while( free-- ) print( "|" );

	// and at last %
	printf( "%3d%%", (uint64_t) (((double) (memory.total - memory.available) / (double) memory.total) * 100.0f) );

	// end of execution
	return 0;
}