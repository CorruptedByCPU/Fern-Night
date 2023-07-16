/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	LIB_JSON
	#define LIB_JSON

	#define	LIB_JSON_TYPE_unknown	0
	#define	LIB_JSON_TYPE_string	1
	#define LIB_JSON_TYPE_number	2
	#define LIB_JSON_TYPE_object	3
	#define LIB_JSON_TYPE_array	4
	#define LIB_JSON_TYPE_boolean	5
	#define	LIB_JSON_TYPE_null	6

	struct LIB_JSON_STRUCTURE {
		uint8_t		type;
		uintptr_t	key;
		uint64_t	value;	// or uintptr_t (depending on type)
		uint64_t	length;	// amount of characters in string or integer (depending on type)
		uintptr_t	next;
	};

	struct LIB_JSON_STRUCTURE lib_json( uint8_t *string );
	uint64_t lib_json_cohesion( uint8_t *json );
	uint8_t lib_json_key( struct LIB_JSON_STRUCTURE json, uint8_t *string );
	uint8_t lib_json_next( struct LIB_JSON_STRUCTURE *json );
	void lib_json_parse( struct LIB_JSON_STRUCTURE *json, uint8_t *string );
#endif