/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_sys_serial_char( uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_CHAR), "D" (character) );
}

void lib_sys_serial_string( uint8_t *string, uint64_t length ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_STRING), "D" (string), "S" (length) );
}

void lib_sys_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_VALUE), "D" (value), "S" (base), "d" (prefix), "c" (character) );
}

//------------------------------------------------------------------------------

void lib_sys_int_serial_char( uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_CHAR), "D" (character) );
}

void lib_sys_int_serial_string( uint8_t *string, uint64_t length ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_STRING), "D" (string), "S" (length) );
}

void lib_sys_int_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	__asm__ volatile( "push %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx" :: "a" (LIB_SYS_REQUEST_SERIAL_VALUE), "D" (value), "S" (base), "d" (prefix), "c" (character) );
}
