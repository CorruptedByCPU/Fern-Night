/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

uint8_t ustarX[ 6 ] = "ustar\x0";

uint8_t lib_tar_check( uintptr_t begin_of_file ) {
	// properties of file
	struct LIB_TAR_STRUCTURE_header *header = (struct LIB_TAR_STRUCTURE_header *) begin_of_file;

	// "ustar" marker?
	if( lib_string_compare( ustarX, header -> magic, 6 ) ) return TRUE;	// yes

	// no
	return FALSE;
}

uint8_t lib_tar_end( uint64_t *data ) {
	// if two consecutive data blocks are emptry
	for( uint8_t i = 0; i < LIB_TAR_BLOCK_SIZE_byte >> STATIC_DIVIDE_BY_QWORD_shift; i++ )
		if( data[ i ] ) return FALSE;	// no

	// yes
	return TRUE;
}