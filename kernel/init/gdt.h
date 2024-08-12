/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_GDT
	#define	KERNEL_INIT_GDT

	struct	KERNEL_INIT_GDT_STRUCTURE {
		uint64_t	null;
		uint64_t	cs_ring0;
		uint64_t	ds_ring0;
		uint64_t	empty;
		uint64_t	ds_ring3;
		uint64_t	cs_ring3;
		uint64_t	tss;
	} __attribute__( (packed) );

	struct	KERNEL_INIT_GDT_STRUCTURE_ENTRY {
		uint16_t	limit_low;
		uint16_t	base_low;
		uint8_t		base_middle;
		uint8_t		access;
		uint8_t		flags_and_limit_high;
		uint8_t		base_high;
	} __attribute__( (packed) );

	struct	KERNEL_INIT_GDT_STRUCTURE_HEADER {
		uint16_t	size;
		uint64_t	address;
	} __attribute__( (packed) );

	// external routines (assembly language)
	extern void kernel_gdt_descriptors( void );

	void kernel_init_gdt();
#endif