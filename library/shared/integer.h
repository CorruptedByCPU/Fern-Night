/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INTEGER
	#define	LIB_INTEGER
	
	uint8_t lib_integer_digit_count( uint64_t value, uint8_t base );
	uint8_t lib_integer_to_string( uint64_t value, uint8_t base, uint8_t *string );
#endif
