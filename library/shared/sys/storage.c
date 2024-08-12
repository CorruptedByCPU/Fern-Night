/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_sys_storage_read_file( struct LIB_SYS_STRUCTURE_STORAGE *storage ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STORAGE_READ_FILE), "D" (storage) );
	return lib_sys_return_empty();
}

//------------------------------------------------------------------------------

void lib_sys_int_storage_read_file( struct LIB_SYS_STRUCTURE_STORAGE *storage ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STORAGE_READ_FILE), "D" (storage) );
	return lib_sys_int_return_empty();
}