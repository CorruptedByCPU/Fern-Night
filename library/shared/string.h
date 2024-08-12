/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_STRING
	#define	LIB_STRING
	
	uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length );
	uint64_t lib_string_length( uint8_t *string );
	uint64_t lib_string_length_line( uint8_t *string );
	uint64_t lib_string_length_scope_digit( uint8_t *string );
	uint64_t lib_string_to_integer( uint8_t *string, uint8_t base );
	uint64_t lib_string_trim( uint8_t string[], uint64_t length, uint8_t move );
	uint64_t lib_string_word( uint8_t string[], uint64_t length );
#endif
