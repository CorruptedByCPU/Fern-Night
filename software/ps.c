/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/integer.h"
	#include	"../library/shared/sys.h"
	//======================================================================

void unit( uint8_t unit ) {
	switch( unit ) {
		case 1:
			print( "KiB" );
			break;
		case 2:
			print( "MiB" );
			break;
		case 3:
			print( "GiB" );
			break;
		case 4:
			print( "TiB" );
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
	// retrieve list of tasks
	struct LIB_SYS_STRUCTURE_TASK *task = (struct LIB_SYS_STRUCTURE_TASK *) lib_sys_task();

	// count tasks and find process with highest ID number (digits)
	uint64_t tasks = EMPTY; uint64_t threads = EMPTY; uint64_t digits = EMPTY;
	while( task[ tasks ].pid && task[ tasks ].length ) {
                // more digits than previous task?
                if( digits < task[ tasks ].pid ) digits = task[ tasks ].pid;    // yes

                // its a task or thread?
                if( task[ tasks++ ].flags & LIB_SYS_TASK_FLAG_thread ) threads++;
        }

	// convert to highest ID to width
	digits = lib_integer_digit_count( digits, STATIC_NUMBER_SYSTEM_decimal ) + 2;	// +2 margin left

        // align PID column
        if( digits > 3 ) for( uint8_t i = 0; i < digits - 3; i++ ) print( " " );

        // show header
	print( "PID Memory   Command\n" );

	// show each entry
	for( uint64_t i = 0; i < tasks; i++ ) {
		// process ID
		int8_t padding = digits - lib_integer_digit_count( task[ i ].pid, STATIC_NUMBER_SYSTEM_decimal );
		for( uint8_t j = 0; j < padding; j++ ) print( " " );
		printf( "%d", task[ i ].pid );

		// memory usage
		print( " " );
		degradation( task[ i ].page << STATIC_PAGE_SIZE_shift );

		// command line
		printf( " %s\n", task[ i ].name );
	}

	// release tasks list
	free( task );

	// end of execution
	return 0;
}