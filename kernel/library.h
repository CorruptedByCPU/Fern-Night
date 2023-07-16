/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_LIBRARY
	#define	KERNEL_LIBRARY

	#define	KERNEL_LIBRARY_BASE_address	0x0000700000000000

	#define	KERNEL_LIBRARY_limit		16

	#define	KERNEL_LIBRARY_FLAG_reserved	1 << 0
	#define	KERNEL_LIBRARY_FLAG_active	1 << 1

	struct	KERNEL_LIBRARY_STRUCTURE {
		uintptr_t	address;
		struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL	*dynsym;
		uint64_t	dynsym_limit;
		uint8_t		*strtab;
		uint16_t	size_page;
		uint16_t	flags;
		uint8_t		length;
		uint8_t		name[ 255 ];
	};

	void kernel_library( struct LIB_ELF_STRUCTURE *elf );
	uintptr_t kernel_library_function( uint8_t *string, uint64_t length );
	void kernel_library_import( struct LIB_ELF_STRUCTURE *elf );
	void kernel_library_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t library_space );
	struct KERNEL_LIBRARY_STRUCTURE *kernel_library_load( uint8_t *path, uint8_t length );
#endif
