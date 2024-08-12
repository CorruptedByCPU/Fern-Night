/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_sys_memory( struct LIB_SYS_STRUCTURE_MEMORY *memory ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY), "D" (memory) );
	return lib_sys_return_empty();
}

uintptr_t lib_sys_memory_alloc( uint64_t bytes ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_ALLOC), "D" (bytes) );
	return lib_sys_return_pointer();
}

void lib_sys_memory_release( uintptr_t address, uint64_t bytes ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_RELEASE), "D" (address), "S" (bytes) );
	return lib_sys_return_empty();
}

uintptr_t lib_sys_memory_share( uintptr_t address, uint64_t bytes, uint64_t pid ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_SHARE), "D" (address), "S" (bytes), "d" (pid) );
	return lib_sys_return_pointer();
}

//------------------------------------------------------------------------------

uintptr_t lib_sys_int_memory_alloc( uint64_t bytes ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_ALLOC), "D" (bytes) );
	return lib_sys_int_return_pointer();
}

void lib_sys_int_memory_release( uintptr_t address, uint64_t bytes ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_RELEASE), "D" (address), "S" (bytes) );
	return lib_sys_int_return_empty();
}

uintptr_t lib_sys_int_memory_share( uintptr_t address, uint64_t bytes, uint64_t pid ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY_SHARE), "D" (address), "S" (bytes), "d" (pid) );
	return lib_sys_int_return_pointer();
}

void lib_sys_int_memory( struct LIB_SYS_STRUCTURE_MEMORY *memory ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_MEMORY), "D" (memory) );
	return lib_sys_int_return_empty();
}