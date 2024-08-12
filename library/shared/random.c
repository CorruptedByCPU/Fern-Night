/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#include	"random.h"
	//======================================================================

uint64_t lib_random() {
	// retrieve next state
	uint64_t random = lib_sys_uptime();

	// make some xor shifts
	random ^= random >> 12;
	random ^= random << 25;
	random ^= random >> 27;

	// return presudorandom value
	return random * UINT64_C( 2685821657736338717 );
}