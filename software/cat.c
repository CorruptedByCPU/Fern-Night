/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/sys.h"
	#include	"../library/shared/string.h"
	//======================================================================

struct LIB_SYS_STRUCTURE_STORAGE arg_file = { EMPTY, EMPTY, EMPTY, EMPTY };

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// if file not selected
	if( argc == 1 ) return LIB_SYS_ERROR_VFS_PATH;

	// set file descriptor
	arg_file.length = lib_string_length( argv[ 1 ] );
	for( uint16_t i = 0; i < arg_file.length; i++ ) arg_file.name[ i ] = argv[ 1 ][ i ];

	// load selected file
	lib_sys_storage_read_file( &arg_file );

	// file loaded?
	if( ! arg_file.address ) return LIB_SYS_ERROR_VFS_FILE_NOT_EXIST;	// no

	// change all unprintable characters to DOTs
	uint8_t *file = (uint8_t *) arg_file.address;
	for( uint64_t i = 0; i < arg_file.size_byte; i++ ) {
		// but leave Tab, Line Feed, Carriage Return - untouched
		if( file[ i ] == STATIC_ASCII_TAB || file[ i ] == STATIC_ASCII_NEW_LINE || file[ i ] == STATIC_ASCII_RETURN ) continue;

		// everything else, proceed
		if( file[ i ] < STATIC_ASCII_SPACE || file[ i ] > STATIC_ASCII_TILDE ) file[ i ] = STATIC_ASCII_DOT;
	}

	// show file content
	print( (const char *) file );

	// end of execution
	return EMPTY;
}
