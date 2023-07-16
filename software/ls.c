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

	// default configuration
	uint8_t	properties = FALSE;
	uint8_t hidden = FALSE;

	// descriptor of current directory
	struct LIB_SYS_STRUCTURE_STORAGE ls_dir = { EMPTY, EMPTY, 1, "." };

uint8_t ls_units[] = { ' ', 'K', 'M', 'G', 'T' };

void ls_format( uint64_t bytes ) {
	// unity type
	uint8_t unit = 0;	// bytes by default
	while( pow( 1024, unit ) < bytes ) unit++;

	// show higher units?
	if( unit > 1 ) {
		printf( " %4.2f", (float) bytes / (float) pow( 1024, unit - 1 ) );
		printf( STATIC_SEQUENCE_COLOR_GRAY"%c ", ls_units[ unit - 1] );
	} else printf( "%9d ", bytes );	// only Bytes
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// args available?
	if( argc > 1 ) for( uint64_t i = 0; i < argc; i++ ) {	// change behavior
		// option?
		if( argv[ i ][ 0 ] == STATIC_ASCII_MINUS ) {
			// options
			uint8_t o = EMPTY;
			while( argv[ i ][ ++o ] ) {
				// show hidden?
				if( argv[ i ][ o ] == STATIC_ASCII_LETTER_a ) hidden = TRUE;	// yes

				// properties of every file?
				if( argv[ i ][ o ] == STATIC_ASCII_LETTER_l ) properties = TRUE;	// yes
			}
		}
	}

	// initialize stream propertries
	struct CONSOLE_STRUCTURE_META console;
	while( lib_sys_stream_get( (uint8_t *) &console, LIB_SYS_STREAM_out ) & LIB_SYS_STREAM_FLAG_undefinied );

	// load image file of cursor
	lib_sys_storage_read_file( &ls_dir );

	// properties of directory
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) ls_dir.address;

	// amount of files to show
	uint64_t c = EMPTY;
	while( vfs[ c ].length ) c++;

	// parse each file
	uint16_t column = 0;
	for( uint64_t i = EMPTY; i < c - 1; i++ ) {
		// don't show hidden files?
		if( ! hidden && vfs[ i ].name[ 0 ] == '.' ) continue;

		// show properties?
		if( properties ) {
			// size of file
			printf( STATIC_SEQUENCE_COLOR_DEFAULT );
			ls_format( vfs[ i ].size );
		}

		// color
		if( vfs[ i ].mode & LIB_VFS_MODE_user_executable ) {
			if( vfs[ i ].type & LIB_VFS_TYPE_regular_file ) print( STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
			if( vfs[ i ].type & LIB_VFS_TYPE_directory ) print( STATIC_SEQUENCE_COLOR_BLUE_LIGHT );
			if( vfs[ i ].type & LIB_VFS_TYPE_symbolic_link ) print( STATIC_SEQUENCE_COLOR_CYAN );
			if( vfs[ i ].type & LIB_VFS_TYPE_shared_object ) print( STATIC_SEQUENCE_COLOR_VIOLET_LIGHT );
		} else print( STATIC_SEQUENCE_COLOR_DEFAULT );

		// properties mode?
		if( properties ) {
			// name of file
			printf( "%s\n", &vfs[ i ].name );

			// next file
			continue;
		}

		// no, default behavior

		// if we do not fit in last column
		if( column + vfs[ i ].length + 2 > console.width ) {
			// but not in new line
			if( console.width - column <= vfs[ i ].length ) print( "\n" );	// move to new line

			// start from first column
			column = 0;
		}

		// show file name
		printf( "%s  ", &vfs[ i ].name );

		// move cursor to next column
		column += vfs[ i ].length + 2;

		// only if there are next files to shown
		if( i + 1 < c ) for( uint16_t t = 0; t < column % (console.width / 4); t++ ) { print( " " ); column++; }
	}

	// end of execution
	return 0;
}
