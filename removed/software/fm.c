/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// driver --------------------------------------------------------------
	#include	"../kernel/driver/ps2.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/sys.h"
	#include	"../library/shared/interface.c"
	// internal ------------------------------------------------------------
	#include	"fm/config.h"
	#include	"fm/data.c"
	//======================================================================

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize engine
	init( argc, argv );

	// main loop
	while( TRUE ) {
		// check for events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &interface );

		// recieve key
		uint16_t key = getkey();

		// release AP time
		lib_sys_sleep( 1 );
	}

	// end of execution
	return 0;
}

void close( void ) {
	// end of execution
	lib_sys_exit();
}

void init( uint64_t argc, uint8_t *argv[] ) {
	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &interface );

	// properties of element with ID 1
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, 1 );
	if( element ) element -> event = (void *) close;

	// properties of element with ID 2
	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *list = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, 2 );
}