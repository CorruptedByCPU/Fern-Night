/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"json.h"
	#include	"string.h"
	//======================================================================

uint64_t lib_json_cohesion( uint8_t *json ) {
	// pointer to formatted json
	uint8_t *formatted = json; uint64_t length = EMPTY;

	// calculate new length
	do {
		// start of quote?
		if( *json == STATIC_ASCII_QUOTATION ) {
			// keep previous value
			uint8_t previous = *(json - 1);

			// count towards
			*(formatted++) = *(json++); length++;

			// look for end quote
			while( *json != STATIC_ASCII_QUOTATION || previous == STATIC_ASCII_BACKSLASH ) {
				// update previous value
				previous = *json;
				
				// count towards
				*(formatted++) = *(json++); length++;
			}

			// count towards
			*(formatted++) = *json; length++;
		} else

		// character visible?
		if( *json > STATIC_ASCII_SPACE && *json < STATIC_ASCII_DELETE ) { *(formatted++) = *json; length++; }
		// else source++;
	} while( *(json++) );

	// enf of formatted json
	*formatted = EMPTY;

	// return new json length
	return length;
}

void lib_json_parse( struct LIB_JSON_STRUCTURE *json, uint8_t *string ) {
	// value
	while( *(++string) != STATIC_ASCII_QUOTATION || *(string++ - 1) == STATIC_ASCII_BACKSLASH );
	json -> value = (uintptr_t) ++string;

	// initial length of next value
	json -> length = EMPTY;

	// search for next key
	switch( *string ) {
		case STATIC_ASCII_BRACLET_OPEN: {
			uint64_t o = TRUE;
			while( o ) {
				json -> length++;
				if( string[ json -> length ] == STATIC_ASCII_BRACLET_OPEN ) o++;
				if( string[ json -> length ] == STATIC_ASCII_BRACLET_CLOSE ) o--;
			}

			json -> type = LIB_JSON_TYPE_object;
			if( string[ ++json -> length ] == STATIC_ASCII_BRACLET_CLOSE ) json -> next = EMPTY;
			else json -> next = (uintptr_t) &string[ json -> length + 1 ];

			break;
		}
		case STATIC_ASCII_QUOTATION: {
			while( string[ ++json -> length ] != STATIC_ASCII_QUOTATION || string[ json -> length - 1 ] == STATIC_ASCII_BACKSLASH );

			json -> type = LIB_JSON_TYPE_string;
			if( string[ ++json -> length ] == STATIC_ASCII_BRACLET_CLOSE ) json -> next = EMPTY;
			else json -> next = (uintptr_t) &string[ json -> length + 1 ];

			json -> value++;
			json -> length -= 2;

			break;
		}
		default: {
			if( *string == STATIC_ASCII_LETTER_n || *string == STATIC_ASCII_LETTER_N ) json -> type = LIB_JSON_TYPE_null;
			else if( *string == STATIC_ASCII_LETTER_t || *string == STATIC_ASCII_LETTER_T || *string == STATIC_ASCII_LETTER_f || *string == STATIC_ASCII_LETTER_F ) { json -> type = LIB_JSON_TYPE_boolean; if( *string == STATIC_ASCII_LETTER_t || *string == STATIC_ASCII_LETTER_T ) json -> value = TRUE; else json -> value = FALSE; }
			else if( *string == STATIC_ASCII_MINUS || *string == STATIC_ASCII_DIGIT_0 || (*string - STATIC_ASCII_DIGIT_0 < STATIC_NUMBER_SYSTEM_decimal) ) { json -> type = LIB_JSON_TYPE_number; json -> value = lib_string_to_integer( (uint8_t *) json -> value, STATIC_NUMBER_SYSTEM_decimal ); }
			else json -> type = LIB_JSON_TYPE_unknown;

			json -> next = EMPTY;
			while( *string && *string != STATIC_ASCII_COMMA ) if( *(string++) == STATIC_ASCII_BRACLET_CLOSE ) return;
			json -> next = (uintptr_t) ++string;
		}
	}
}

uint8_t lib_json_key( struct LIB_JSON_STRUCTURE json, uint8_t *string ) {
	// key name
	uint8_t *key = (uint8_t *) json.key + 1;

	// length of key name
	uint64_t length = lib_string_length( string );

	// check
	while( *key != STATIC_ASCII_QUOTATION ) if( *(key++) == *(string++) ) length--; else return FALSE;

	// key recognized?
	if( ! length && *key == STATIC_ASCII_QUOTATION ) return TRUE;	// yes

	// no
	return FALSE;
}

uint8_t lib_json_next( struct LIB_JSON_STRUCTURE *json ) {
	// next object exist?
	if( ! json -> next ) return FALSE;	// no

	// key
	json -> key = json -> next;

	// set other properties
	lib_json_parse( json, (uint8_t *) json -> key );

	// done
	return TRUE;
}

struct LIB_JSON_STRUCTURE lib_json( uint8_t *string ) {
	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = { EMPTY };

	// object?
	if( *string != STATIC_ASCII_BRACLET_OPEN ) return json;	// incorrect JSON structure

	// key
	json.key = (uintptr_t) ++string;

	// set other properties
	lib_json_parse( (struct LIB_JSON_STRUCTURE *) &json, string );

	// return JSON structure
	return json;
}
