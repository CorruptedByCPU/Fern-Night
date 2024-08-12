/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t lib_sys_exec( uint8_t *path, uint16_t length, uint8_t stream ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_EXEC), "D" (path), "S" (length), "d" (stream) );
	return lib_sys_return_value_unsigned();
}

//------------------------------------------------------------------------------

uint64_t lib_sys_int_exec( uint8_t *path, uint16_t length, uint8_t stream ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_EXEC), "D" (path), "S" (length), "d" (stream) );
	return lib_sys_int_return_value_unsigned();
}
