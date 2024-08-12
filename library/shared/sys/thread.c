/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t lib_sys_thread( uintptr_t function, uint8_t *string, uint8_t length ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_THREAD), "D" (function), "S" (string), "d" (length) );
	return	lib_sys_return_value();
}

//------------------------------------------------------------------------------

uint64_t lib_sys_int_thread( uintptr_t function, uint8_t *string, uint8_t length ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_THREAD), "D" (function), "S" (string), "d" (length) );
	return	lib_sys_int_return_value();
}