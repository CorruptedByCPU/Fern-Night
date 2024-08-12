/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_sys_stream_out( uint8_t *string, uint64_t length ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_OUT), "D" (string), "S" (length) );
}

void lib_sys_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_OUT_VALUE), "D" (value), "S" (base), "d" (prefix), "c" (character) );
}

uint64_t lib_sys_stream_in( uint8_t *cache ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STREAM_IN), "D" (cache) );
	return lib_sys_return_value_unsigned();
}

void	lib_sys_stream_set( uint8_t *meta, uint8_t stream ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_SET), "D" (stream), "S" (meta) );
}

uint8_t lib_sys_stream_get( uint8_t *meta, uint8_t stream ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STREAM_GET), "D" (meta), "S" (stream) );
	return lib_sys_return_value_unsigned();
}

//------------------------------------------------------------------------------

void lib_sys_int_stream_out( uint8_t *string, uint64_t length ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_OUT), "D" (string), "S" (length) );
}

void lib_sys_int_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_OUT_VALUE), "D" (value), "S" (base), "d" (prefix), "c" (character) );
}

uint64_t lib_sys_int_stream_in( uint8_t *cache ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STREAM_IN), "D" (cache) );
	return lib_sys_int_return_value_unsigned();
}

void	lib_sys_int_stream_set( uint8_t *meta, uint8_t stream ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_STREAM_SET), "D" (stream), "S" (meta) );
}

uint8_t lib_sys_int_stream_get( uint8_t *meta, uint8_t stream ) {
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_STREAM_GET), "D" (meta), "S" (stream) );
	return lib_sys_int_return_value_unsigned();
}