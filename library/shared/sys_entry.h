/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

// main function of every process
int main( void );

#ifndef	LIB_SYS_ENTRY
	#define	LIB_SYS_ENTRY

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#include	"../../default.h"
	//----------------------------------------------------------------------

	void entry( void ) {
		// sad hack :|
		__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

		// evecute software main routine
		main();

		// hold the door
		lib_sys_exit();
	}
#endif