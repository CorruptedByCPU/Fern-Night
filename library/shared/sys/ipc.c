/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_sys_ipc_send( uint64_t pid, uint8_t *data ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_IPC_SEND), "D" (pid), "S" (data) );
	return lib_sys_return_empty();
}

uint8_t lib_sys_ipc_receive( struct LIB_SYS_STRUCTURE_IPC *message, uint8_t type ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_IPC_RECEIVE), "D" (message), "S" (type) );
	return lib_sys_return_bool();
}

//------------------------------------------------------------------------------

void lib_sys_int_ipc_send( uint64_t pid, uint8_t *data ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_IPC_SEND), "D" (pid), "S" (data) );
	return lib_sys_int_return_empty();
}

uint8_t lib_sys_int_ipc_receive( struct LIB_SYS_STRUCTURE_IPC *message, uint8_t type ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_IPC_RECEIVE), "D" (message), "S" (type) );
	return lib_sys_int_return_bool();
}
