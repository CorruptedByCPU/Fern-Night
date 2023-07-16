/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"vfs.h"
	//======================================================================

uint8_t lib_vfs_check( uintptr_t address, uint64_t size_byte ) {
	// properties of file
	uint32_t *vfs = (uint32_t *) address;

	// magic value?
	if( vfs[ (size_byte >> STATIC_DIVIDE_BY_4_shift) - 1 ] == LIB_VFS_magic ) return TRUE;	// yes

	// no
	return FALSE;
}

struct KERNEL_STORAGE_STRUCTURE_FILE lib_vfs_file( struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t length ) {
	// check one by one files preserved in VFS file
	do if( vfs -> length == length && lib_string_compare( name, (uint8_t *) &vfs -> name, length ) )
		return (struct KERNEL_STORAGE_STRUCTURE_FILE) { (uint64_t) vfs, vfs -> size };
	while( vfs++ -> length );

	// file not found
	return (struct KERNEL_STORAGE_STRUCTURE_FILE) { EMPTY };
}

void lib_vfs_init( struct LIB_VFS_STRUCTURE *vfs ) {
	// package begining pointer
	uint64_t offset = (uint64_t) vfs;

	// set correct file content offsets, related to package inside memory
	while( vfs -> length ) {
		vfs++ -> offset += offset;
	}
}

void lib_vfs_read( struct LIB_VFS_STRUCTURE *knot, uintptr_t destination ) {
	// copy content of file to destination
	uint8_t *source = (uint8_t *) knot -> offset;
	uint8_t *target = (uint8_t *) destination;
	for( uint64_t i = 0; i < knot -> size; i++ ) target[ i ] = source[ i ];
}