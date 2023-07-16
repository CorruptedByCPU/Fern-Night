/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"elf.h"
	//======================================================================

uint8_t lib_elf_check( uintptr_t address ) {
	// properties of file
	struct LIB_ELF_STRUCTURE *header = (struct LIB_ELF_STRUCTURE *) address;

	// "ELF" marker?
	if( header -> magic_number != LIB_ELF_HEADER_magic_number ) return FALSE;	// no

	// yes
	return TRUE;
}