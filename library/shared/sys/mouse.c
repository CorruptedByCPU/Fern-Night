/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void lib_sys_mouse( uintptr_t mouse ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MOUSE), "D" (mouse) );
	return lib_sys_return_empty();
}

//------------------------------------------------------------------------------

void lib_sys_int_mouse( uintptr_t mouse ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MOUSE), "D" (mouse) );
	return lib_sys_int_return_empty();
}