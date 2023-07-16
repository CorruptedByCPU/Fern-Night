/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"integer.h"
	//======================================================================

uint8_t lib_integer_digit_count( uint64_t value, uint8_t base ) {
	// if unsupported number system
	if( base < 2 ) return ZERO;	// return ZERO

	// number in digits
	uint8_t digits = 0;

	// parse every digit from value
	while( value ) {
		// even 
		digits++;

		// remove digit from value
		value /= base;
	}

	// return number of digits
	if( ! digits ) return digits + 1;
	return digits;
}

// length of "string" must be 64 characters
uint8_t lib_integer_to_string( uint64_t value, uint8_t base, uint8_t *string ) {
	// if unsupported number system
	if( base < 2 || base > 36 ) return ZERO;	// return ZERO

	// length of string
	uint8_t length = lib_integer_digit_count( value, base );

	// value equal to ZERO?
	if( ! value ) { string[ value ] = STATIC_ASCII_DIGIT_0; return length; }

	// digit index on string
	uint8_t index = 0;

	// parse every digit from value
	while( value ) {
		// prepare place for digit
		index++;

		// store first digit from value
		string[ length - index ] = (value % base) | STATIC_ASCII_DIGIT_0;

		// if digit is alfanumerical
		if( string[ length - index ] > STATIC_ASCII_DIGIT_9 ) string[ length - index ] += 0x07;	// ASCII "A"

		// remove digit from value
		value /= base;
	}

	// return string length
	return length;
}