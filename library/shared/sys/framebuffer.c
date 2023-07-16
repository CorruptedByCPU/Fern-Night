/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void lib_sys_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_FRAMEBUFFER), "D" (framebuffer) );
	return lib_sys_return_empty();
}

//------------------------------------------------------------------------------

void lib_sys_int_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_FRAMEBUFFER), "D" (framebuffer) );
	return lib_sys_int_return_empty();
}