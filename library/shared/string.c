/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"string.h"
	//======================================================================

uint8_t lib_string_compare( uint8_t *source, uint8_t *target, uint64_t length ) {
	// porównaj obydwa ciągi
	for( uint64_t i = 0; i < length; i++ )
		// znaleziono różnicę?
		if( source[ i ] != target[ i ] ) return FALSE;

	// równe sobie
	return TRUE;
}

uint64_t lib_string_length( uint8_t *string ) {
	// length of string
	uint64_t length = 0;

	// increment until empty
	while( *string++ ) length++;

	// return length of string
	return length;
}

uint64_t lib_string_length_line( uint8_t *string ) {
	// length of line
	uint64_t length = 0;

	// increment until empty of STATIC_ASCII_NEW_LINE found
	while( TRUE ) {
		// end of string?
		if( ! string[ length ] ) break;	// yes

		// new line marker?
		if( string[ length ] == STATIC_ASCII_NEW_LINE ) break;	// yes

		// next character from string
		length++;
	}

	// return length of string
	return length;
}

uint64_t lib_string_length_scope_digit( uint8_t *string ) {
	// amount of digits in string
	uint64_t length = 0;

	// increment until empty
	while( *string == STATIC_ASCII_MINUS || (*string >= STATIC_ASCII_DIGIT_0 && *string <= STATIC_ASCII_DIGIT_9) ) { string++; length++; }

	// return string length
	return length;
}

uint64_t lib_string_to_integer( uint8_t *string, uint8_t base ) {
	// calculated value
	uint64_t value = EMPTY;

	// if unsupported number system
	if( base < 2 || base > 36 ) return ZERO;	// return ZERO

	// amount of digits to convert
	uint64_t index = lib_string_length_scope_digit( string );

	// empty string?
	if( index == STATIC_MAX_unsigned ) return ZERO;	// yes

	// first order of magnitude power
	uint64_t order_of_magnitude = 1;

	// calulate
	while( index != EMPTY ) {
		// retrieve digit from value
		uint8_t digit = string[ --index ];

		// last digit is a "sign"?
		if( digit == STATIC_ASCII_MINUS && base == STATIC_NUMBER_SYSTEM_decimal )
			// return value with sign
			return ~value + 1;

		// digit of supported base?
		if( digit >= STATIC_ASCII_DIGIT_0 && digit <= STATIC_ASCII_DIGIT_9) {
			// fix digit
			digit -= STATIC_ASCII_DIGIT_0;
		}
		else if( digit >= STATIC_ASCII_LETTER_A && digit <= STATIC_ASCII_LETTER_Z ) {
			// fix digit
			digit -= STATIC_ASCII_DIGIT_0 + (STATIC_ASCII_LETTER_A - STATIC_ASCII_DIGIT_9);
		}
		else if( digit >= STATIC_ASCII_LETTER_a && digit <= STATIC_ASCII_LETTER_z ) {
			// fix digit
			digit -= STATIC_ASCII_DIGIT_0 + (STATIC_ASCII_LETTER_a - STATIC_ASCII_DIGIT_0);
		}
		else return EMPTY;	// invalid digit base

		// calculate value of the first order of magnitude
		value += digit * order_of_magnitude;

		// next order of magnitude
		order_of_magnitude *= base;
	}

	// return value
	return value;
}

uint64_t lib_string_trim( uint8_t string[], uint64_t length, uint8_t move ) {
	// search the string from the beginning
	uint64_t i = 0;

	// check beginning
	for( ; i < length; i++ )
		// printable character?
		if( string[ i ] > STATIC_ASCII_SPACE && string[ i ] < STATIC_ASCII_DELETE ) break;

	// check end
	while( i != length )
		// printable character?
		if( string[ length - 1 ] > STATIC_ASCII_SPACE && string[ length - 1 ] < STATIC_ASCII_DELETE ) break;
		else length--;

	// new string size
	length -= i;

	// move contents of string to beginning (if needed)
	if( move ) for( uint64_t k = 0; i && k < length; k++ )
		string[ k ] = string[ k + i ];

	// return new string size
	return length;
}

uint64_t lib_string_word( uint8_t string[], uint64_t length ) {
	// search from the beginning
	for( uint64_t i = 0; i < length; i++ )
		// nonprinting character?
		if( string[ i ] <= STATIC_ASCII_SPACE || string[ i ] >= STATIC_ASCII_DELETE ) return i;

	// the whole string is "word"
	return length;
}
