/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_MEMORY
	#define	KERNEL_MEMORY

	uintptr_t kernel_memory_alloc( uint64_t pages );
	uintptr_t kernel_memory_alloc_page( void );
	uint64_t kernel_memory_acquire( uint32_t *memory_map, uint64_t N );
	void kernel_memory_dispose( uint32_t *memory_map, uint64_t p, uint64_t N );
	void kernel_memory_release( uintptr_t address, uint64_t pages );
	void kernel_memory_release_page( uintptr_t address );
	uintptr_t kernel_memory_share( uintptr_t physical_address, uint64_t size_byte );
#endif