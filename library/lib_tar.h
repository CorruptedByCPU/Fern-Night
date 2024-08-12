/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_TAR
	#define	LIB_TAR

	#define	LIB_TAR_BLOCK_SIZE_shift	STATIC_DIVIDE_BY_512_shift
	#define	LIB_TAR_BLOCK_SIZE_byte		512

	struct LIB_TAR_STRUCTURE_header {
		uint8_t name[ 100 ];
		uint8_t mode[ 8 ];
		uint8_t uid[ 8 ];
		uint8_t gid[ 8 ];
		uint8_t size[ 12 ];
		uint8_t mtime[ 12 ];
		uint8_t chksum[ 8 ];
		uint8_t typeflag;
		uint8_t linkname[ 100 ];
		uint8_t magic[ 6 ];
		uint8_t version[ 2 ];
		uint8_t uname[ 32 ];
		uint8_t gname[ 32 ];
		uint8_t devmajor[ 8 ];
		uint8_t devminor[ 8 ];
		uint8_t prefix[ 155 ];
	};

	uint8_t lib_tar_check( uintptr_t address );
	uint8_t lib_tar_end( uint64_t *data );
#endif