/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uintptr_t lib_sys_task() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK) );
	return lib_sys_return_pointer();
}

uint64_t lib_sys_task_pid() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK_PID) );
	return lib_sys_return_value_unsigned();
}

uint16_t lib_sys_task_status( uint64_t pid ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK_STATUS), "D" (pid) );
	return	lib_sys_return_value();
}

//------------------------------------------------------------------------------

uintptr_t lib_sys_int_task() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK) );
	return lib_sys_int_return_pointer();
}

uint64_t lib_sys_int_task_pid() {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK_PID) );
	return lib_sys_int_return_value_unsigned();
}

uint16_t lib_sys_int_task_status( uint64_t pid ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_TASK_STATUS), "D" (pid) );
	return	lib_sys_int_return_value();
}