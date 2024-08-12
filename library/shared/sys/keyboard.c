/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint16_t lib_sys_keyboard() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_KEYBOARD) );
	return lib_sys_return_value();
}

//------------------------------------------------------------------------------

uint16_t lib_sys_int_keyboard() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_KEYBOARD) );
	return lib_sys_int_return_value();
}