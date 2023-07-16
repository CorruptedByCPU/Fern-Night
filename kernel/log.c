/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_log( const char *string, ... ) {
	// properties of argument list
	va_list args;
	va_start( args, string );

	// for every character from string
	for( uint64_t i = 0; string[ i ] != EMPTY; i++ ) {
		// sequence?
		if( string[ i ] == STATIC_ASCII_PERCENT && string[ i + 1 ] != EMPTY ) {
			// just a PERCENT character?
			switch( string[ i + 1 ] ) {
				case STATIC_ASCII_PERCENT: { driver_serial_char( STATIC_ASCII_PERCENT ); break; }
				case STATIC_ASCII_LETTER_c: { driver_serial_char( va_arg( args, uint64_t ) ); break; }
				case STATIC_ASCII_LETTER_d: { driver_serial_value( va_arg( args, uint64_t ), STATIC_NUMBER_SYSTEM_decimal, 1, STATIC_ASCII_DIGIT_0 ); break; }
				case STATIC_ASCII_LETTER_s: { driver_serial_string( va_arg( args, uint8_t * ), FALSE ); break; }
				case STATIC_ASCII_LETTER_x: { driver_serial_value( va_arg( args, uint64_t ), STATIC_NUMBER_SYSTEM_hexadecimal, 1, STATIC_ASCII_DIGIT_0 ); break; }
			}

			// omit sequence
			i++;
		} else
			// nope, print character
			driver_serial_char( string[ i ] );
	}

	// end of properties of arguemnt list
	va_end( args );
}