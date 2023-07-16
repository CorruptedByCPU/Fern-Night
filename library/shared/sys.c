/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// sys configuration ---------------------------------------------------
	#include	"string.h"
	#include	"string.c"
	#include	"sys.h"
	// default rountines ---------------------------------------------------
	#include	"sys/default.c"
	#include	"sys/exec.c"
	#include	"sys/framebuffer.c"
	#include	"sys/ipc.c"
	#include	"sys/keyboard.c"
	#include	"sys/memory.c"
	#include	"sys/mouse.c"
	#include	"sys/task.c"
	#include	"sys/thread.c"
	#include	"sys/time.c"
	#include	"sys/serial.c"
	#include	"sys/storage.c"
	#include	"sys/stream.c"
	//======================================================================

void lib_sys_exit() {
	// execute leave out routine
	__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_EXIT) );
	return lib_sys_return_empty();
}

//------------------------------------------------------------------------------
// substitute of libc

void *malloc( size_t byte ) {
	// assign place for area size
	uintptr_t *address = (uintptr_t *) lib_sys_memory_alloc( byte + (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) );

	// register area size
	*address = MACRO_PAGE_ALIGN_UP( byte + (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) ) >> STATIC_PAGE_SIZE_shift;

	// return pointer aligned to 0x10 (SSE, MMX, AVX etc. requirement)
	return &address[ 2 ];
}

void *realloc( uintptr_t source, size_t byte ) {
	// size of current space
	uint64_t *space = (uint64_t *) MACRO_PAGE_ALIGN_DOWN( source );
	uint64_t length = (space[ 0 ] << STATIC_PAGE_SIZE_shift) - (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift);

	// do we need wider space?
	if( byte > length - (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) ) {
		uintptr_t *new = (uintptr_t *) malloc( byte + (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) );

		memcpy( (void *) new, (void *) source, length );

		free( (void *) source );

		return (void *) new;
	} else return (void *) source;
}

void exit() {
	// end of process
	void lib_sys_exit();
}

double strtof( uint8_t *string, uint64_t length ) {
	// check if value is signed
	int8_t mark = FALSE;	// by default no
	if( string[ 0 ] == STATIC_ASCII_MINUS ) { mark = TRUE; ++string; }	// yes

	// amount of digits before dot precision
	uint64_t prefix_length = lib_string_length_scope_digit( string );
	// and value representing that digits
	int64_t prefix_value = lib_string_to_integer( string, STATIC_NUMBER_SYSTEM_decimal );
	// keep value as double point
	double prefix = (double) prefix_value;

	// move string pointer after dot precision
	string += prefix_length + 1;

	// amount of digits after dot precision
	uint64_t result_length = lib_string_length_scope_digit( string );
	// and value representing that digits
	int64_t result_value = lib_string_to_integer( string, STATIC_NUMBER_SYSTEM_decimal );
	// keep value as double point
	double result = (double) result_value;

	// move all digits of result to behind dot precision if not ZERO
	if( result_value ) while( result_length-- ) result /= 10.f;

	// connect prefix and result regarded of sign mark
	result += prefix;
	if( mark ) result = -result;

	// return double point value
	return result;
}

void free( void *address ) {
	// retrieve area size
	uintptr_t *size = address - (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift);

	// release assigned area
	lib_sys_memory_release( (uintptr_t) size, *size << STATIC_PAGE_SIZE_shift );
}

uint64_t abs( int64_t i ) {
	return i < 0 ? -i : i;
}

double fmod( double x, double y ) {
	return x - (int64_t) (x / y) * y;
}

float sqrtf( float x ) {
	union {
		int64_t	i;
		float	x;
	} u;

	u.x = x;
	u.i = (1 << 29) + (u.i >> 1) - (1 << 22);

	u.x = u.x + x / u.x;
	u.x = 0.25f * u.x + x / u.x;

	return u.x;
}

double maxf( double first, double second ) {
	if( first > second ) return first;
	else return second;
}

void print( const char *string ) {
	lib_sys_stream_out( (uint8_t *) string, lib_string_length( (uint8_t *) string ) );
}

uint16_t getkey() {
	// prepare temporary message cache
	struct LIB_SYS_STRUCTURE_IPC ipc;

	// receive pending messages
	if( ! lib_sys_ipc_receive( (struct LIB_SYS_STRUCTURE_IPC *) &ipc, LIB_SYS_IPC_TYPE_KEYBOARD ) ) return EMPTY;

	// message properties
	struct LIB_SYS_STRUCTURE_IPC_KEYBOARD *keyboard = (struct LIB_SYS_STRUCTURE_IPC_KEYBOARD *) &ipc.data;

	// return key
	return keyboard -> key;
}

void *memcpy( void *restrict dstptr, const void *restrict srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

void *memset( void *bufptr, int64_t value, size_t size ) {
	uint8_t* buf = (uint8_t *) bufptr;

	for( size_t i = 0; i < size; i++ )
		buf[ i ] = (uint8_t) value;

	return bufptr;
}

void log( const char *string, ... ) {
	// properties of argument list
	va_list argv;
	va_start( argv, string );

	// for every character from string
	uint64_t length = lib_string_length( (uint8_t *) string );
	for( uint64_t i = 0; i < length; i++ ) {
		// sequence?
		if( string[ i ] == STATIC_ASCII_PERCENT ) {
			// check sequence type
			i++;

			// prefix before type?
			uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ i ] );
			uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ i ], STATIC_NUMBER_SYSTEM_decimal );

			// omit prefix if exist
			i += prefix;

			// check sequence type
			switch( string[ i ] ) {
				case '%': {
					// show '%' character
					lib_sys_serial_char( STATIC_ASCII_PERCENT );

					// next character from string
					break;
				}
				case 'd': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// value is signed?
					if( value & STATIC_QWORD_sign ) {
						value = abs( value );
						lib_sys_serial_char( STATIC_ASCII_MINUS );
					}

					// show 'value'
					lib_sys_serial_value( value, STATIC_NUMBER_SYSTEM_decimal, p_value, STATIC_ASCII_SPACE );

					// next character from string
					continue;
				}
				case '.':
				case 'f': {
					// prefix before type?
					uint64_t suffix = lib_string_length_scope_digit( (uint8_t *) &string[ i + 1 ] );
					uint64_t s_value = lib_string_to_integer( (uint8_t *) &string[ i + 1 ], STATIC_NUMBER_SYSTEM_decimal );

					// value
					double f = va_arg( argv, double );
					if( f < 0.0f ) { f = -f; lib_sys_serial_char( STATIC_ASCII_MINUS ); }

					// number of digits after dot
					uint64_t s_digits = 1;
					if( s_value ) for( uint8_t m = 0; m < s_value; m++ ) s_digits *= 10;
					else s_digits = 1000000;	// if not specified set default

					// show 'value'
					lib_sys_serial_value( (uint64_t) f, STATIC_NUMBER_SYSTEM_decimal, 1, STATIC_ASCII_SPACE );
					lib_sys_serial_char( STATIC_ASCII_DOT );
					lib_sys_serial_value( (uint64_t) ((double) (f - (uint64_t) f) * (double) s_digits), STATIC_NUMBER_SYSTEM_decimal, s_value, STATIC_ASCII_DIGIT_0 );

					// omit suffix and dot if exist
					i += suffix;

					// next character from string
					break;
				}
				case 'x': {
					// show 'value'
					lib_sys_serial_value( va_arg( argv, uint64_t ), STATIC_NUMBER_SYSTEM_hexadecimal, 1, FALSE );

					// next character from string
					continue;
				}
				case 'c': {
					// show 'character'
					lib_sys_serial_char( (uint8_t) va_arg( argv, uint64_t ) );

					// next character from string
					continue;
				}
				case 's': {
					// set pointer to string
					uint8_t *substring = va_arg( argv, uint8_t * );

					// send whole string
					lib_sys_serial_string( substring, lib_string_length( (uint8_t *) substring ) );

					// next character from string
					continue;
				}
			}

			// sequence parsed or unrecognized
			i++;

			// next sequence
			continue;
		}

		// nope, print characters up to next sequence occurrence
		uint64_t k = i; for( ; k < length; k++ ) if( string[ k ] == STATIC_ASCII_PERCENT ) break;
		lib_sys_serial_string( (uint8_t *) &string[ i ], k - i );

		// string parsed
		i = k - 1;
	}

	// end of properties of arguemnt list
	va_end( argv );
}

uint64_t pow( uint64_t base, uint64_t exponent ) {
	// resulf by default
	uint64_t result = 1;

	// until exponent exist
	while( exponent ) {
		// calculate
		if( (exponent & 1) == 1) result *= base;

		// remove exponent fraction
		exponent >>= 1;

		// change power of
		base *= base;
	}

	// return result
	return result;
}

void printf( const char *string, ... ) {
	// properties of argument list
	va_list argv;
	va_start( argv, string );

	// for every character from string
	uint64_t length = lib_string_length( (uint8_t *) string );
	for( uint64_t i = 0; i < length; i++ ) {
		// sequence?
		if( string[ i ] == STATIC_ASCII_PERCENT ) {
			// check sequence type
			i++;

			// prefix before type?
			uint64_t prefix = lib_string_length_scope_digit( (uint8_t *) &string[ i ] );
			uint64_t p_value = lib_string_to_integer( (uint8_t *) &string[ i ], STATIC_NUMBER_SYSTEM_decimal );

			// omit prefix if exist
			i += prefix;

			// check sequence type
			switch( string[ i ] ) {
				case '%': {
					// show '%' character
					lib_sys_stream_out( (uint8_t *) &string[ i ], TRUE );

					// next character from string
					break;
				}
				case 'c': {
					// show 'character'
					uint8_t character = va_arg( argv, uint64_t );
					lib_sys_stream_out( (uint8_t *) &character, 1 );

					// next character from string
					continue;
				}
				case 'd': {
					// retrieve value
					uint64_t value = va_arg( argv, uint64_t );

					// value is signed?
					if( value & STATIC_QWORD_sign ) {
						value = abs( value );
						lib_sys_stream_out( (uint8_t *) "-", 1 );
					}

					// show 'value'
					lib_sys_stream_out_value( value, STATIC_NUMBER_SYSTEM_decimal, p_value, STATIC_ASCII_SPACE );

					// next character from string
					continue;
				}
				case '.':
				case 'f': {
					// prefix before type?
					uint64_t suffix = lib_string_length_scope_digit( (uint8_t *) &string[ i + 1 ] );
					uint64_t s_value = lib_string_to_integer( (uint8_t *) &string[ i + 1 ], STATIC_NUMBER_SYSTEM_decimal );

					// value
					double f = (double) va_arg( argv, double );
					if( f < 0.0f ) { f = -f; lib_sys_stream_out( (uint8_t *) "-", 1 ); }

					// number of digits after dot
					uint64_t s_digits = 1;
					if( s_value ) for( uint8_t m = 0; m < s_value; m++ ) s_digits *= 10;
					else s_digits = 1000000;	// if not specified set default

					// show 'value'
					lib_sys_stream_out_value( (uint64_t) f, STATIC_NUMBER_SYSTEM_decimal, 1, STATIC_ASCII_SPACE );
					lib_sys_stream_out( (uint8_t *) ".", 1 );
					lib_sys_stream_out_value( (uint64_t) ((double) (f - (uint64_t) f) * (double) s_digits), STATIC_NUMBER_SYSTEM_decimal, s_value, STATIC_ASCII_DIGIT_0 );

					// omit suffix and dot if exist
					i += suffix;

					// next character from string
					break;
				}
				case 's': {
					// set pointer to string
					uint8_t *substring = va_arg( argv, uint8_t * );

					// send whole string
					lib_sys_stream_out( (uint8_t *) substring, lib_string_length( (uint8_t *) substring ) );

					// next character from string
					continue;
				}
			}

			// sequence parsed or unrecognized
			i++;

			// next sequence
			continue;
		}

		// nope, print characters up to next sequence occurrence
		uint64_t k = i; for( ; k < length; k++ ) if( string[ k ] == STATIC_ASCII_PERCENT ) break;
		lib_sys_stream_out( (uint8_t *) &string[ i ], k - i );

		// string parsed
		i = k - 1;
	}

	// end of properties of arguemnt list
	va_end( argv );
}

//==============================================================================
