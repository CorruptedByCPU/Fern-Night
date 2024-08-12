/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t lib_sys_time() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TIME) );
	return lib_sys_return_value_unsigned();
}

void lib_sys_sleep( uint64_t ticks ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_SLEEP), "D" (ticks) );
	return lib_sys_return_empty();
}

uint64_t lib_sys_uptime() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_UPTIME) );
	return lib_sys_return_value_unsigned();
}

//------------------------------------------------------------------------------

uint64_t lib_sys_int_time() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TIME) );
	return lib_sys_int_return_value_unsigned();
}

void lib_sys_int_sleep( uint64_t ticks ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_SLEEP), "D" (ticks) );
	return lib_sys_int_return_empty();
}

uint64_t lib_sys_int_uptime() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_UPTIME) );
	return lib_sys_int_return_value_unsigned();
}