/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/interface.h"
	#include	"../library/shared/interface.c"
	#include	"../library/shared/json.h"
	#include	"../library/shared/json.c"
	#include	"../library/shared/string.h"
	#include	"../library/shared/sys.h"
	#include	"../library/shared/sys.c"
	//======================================================================

	// uint8_t debug_interface[] = "...";
	MACRO_IMPORT_FILE_AS_STRING( debug_interface, "software/debug.json" );

	struct LIB_INTERFACE_STRUCTURE interface = { (uintptr_t) &debug_interface, 5, "Debug" };

void debug_close( void ) {
	// end of execution
	lib_sys_exit();
}

void debug_thread( void ) {
	// hold the door
	while( TRUE );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &interface );

	// to elements with IDs 1..4, connect function close();
	for( uint8_t i = 1; i <=4; i++ ) {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, i );
		if( element ) element -> event = (void *) debug_close;
	}

	// create new thread
	uint8_t thread[] = " debug_thread( void );";
	lib_sys_thread( (uintptr_t) &debug_thread, (uint8_t *) &thread, sizeof( thread ) - 1 );

	// main loop
	while( TRUE ) {
		// check for events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &interface );
	}

	// end of execution
	return 0;
}
