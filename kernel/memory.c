/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

uintptr_t kernel_memory_share( uintptr_t physical_address, uint64_t size_byte ) {
	// find free space in process memory for framebuffer
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// convert size of framebuffer into pages
	uint64_t pages = MACRO_PAGE_ALIGN_UP( size_byte ) >> STATIC_PAGE_SIZE_shift;

	// search memory map for N continuous pages
	for( uint64_t i = 0; i < KERNEL_VIRTUAL_SIZE_byte << STATIC_MULTIPLE_BY_8_shift; i++ ) {
		// by default, they are available
		uint8_t flag = TRUE;
		for( uint64_t j = i; j < i + pages; j++ )
			// unavailable Bit in queue?
			if( ! (task -> memory_map[ j / STATIC_DWORD_SIZE_bit ] & 1 << j % STATIC_DWORD_SIZE_bit) ) {
				// move the search after an unavailable bit
				i = j; flag = FALSE; break;
			}

		// found free space?
		if( flag ) {
			// mark pages as reserved
			for( uint64_t j = i; j < i + pages; j++ ) task -> memory_map[ j / STATIC_DWORD_SIZE_bit ] &= ~(1 << j % STATIC_DWORD_SIZE_bit );

			// map framebuffer space to process memory
			kernel_page_map( (uintptr_t *) task -> cr3, physical_address, (uintptr_t) i << STATIC_PAGE_SIZE_shift, pages, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

			// return the address of the first page in the collection
			return (uintptr_t) i << STATIC_PAGE_SIZE_shift;
		}
	}

	// no free space
	return EMPTY;
}

void kernel_memory_release_page( uintptr_t address ) {
	// release page from binary memory map
	kernel_memory_release( address | KERNEL_PAGE_mirror, 1 );
}

void kernel_memory_release( uintptr_t address, uint64_t N ) {
	// clean page before releasing
	// kernel guarantees clean pages on allocation
	kernel_page_clean( address, N );

	// release occupied pages inside kernels binary memory map
	kernel_memory_dispose( kernel -> memory_base_address, (address & ~KERNEL_PAGE_mirror) >> STATIC_PAGE_SIZE_shift, N );

	// more available pages
	kernel -> page_available += N;
}

uintptr_t kernel_memory_alloc_page( void ) {
	// alloc only 1 page
	return kernel_memory_alloc( 1 ) & ~KERNEL_PAGE_mirror;
}

uintptr_t kernel_memory_alloc( uint64_t pages ) {
	// block access to binary memory map (only one at a time)
	while( __sync_val_compare_and_swap( &kernel -> memory_page_semaphore, UNLOCK, LOCK ) );

	// search for requested size of space
	uint64_t page = EMPTY;
	if( (page = kernel_memory_acquire( kernel -> memory_base_address, pages )) ) {
		// less available pages
		kernel -> page_available -= pages;

		// convert first page number to logical address
		page = (page << STATIC_PAGE_SIZE_shift) | KERNEL_PAGE_mirror;
	}

	// unlock access to binary memory map
	kernel -> memory_page_semaphore = UNLOCK;

	// return space address
	return (uintptr_t) page;
}

uint64_t kernel_memory_acquire( uint32_t *memory_map, uint64_t N ) {
	// search binary memory map for N continuous (p)ages
	for( uint64_t p = 0; p < kernel -> page_limit; p++ ) {
		// by default we found N enabled bits
		uint8_t found = TRUE;

		// check N (c)onsecutive pages
		for( uint64_t c = p; c < p + N; c++ ) {
			// broken continous?
			if( ! (memory_map[ c / STATIC_DWORD_SIZE_bit ] & 1 << c % STATIC_DWORD_SIZE_bit) ) {
				// one of the bits is disabled
				found = FALSE;

				// start looking from next position
				p = c;

				// restart
				break;
			}
		}

		// if N consecutive pages have been found
		if( found ) {
			// mark pages as (r)eserved
			for( uint64_t r = p; r < p + N; r++ )
				memory_map[ r / STATIC_DWORD_SIZE_bit ] &= ~(1 << r % STATIC_DWORD_SIZE_bit );

			// return address of allocated memory area
			return p;
		}
	}

	// no available memory area
	return EMPTY;
}

void kernel_memory_dispose( uint32_t *memory_map, uint64_t p, uint64_t N ) {
	// mark pages as (f)ree
	for( uint64_t f = p; f < p + N; f++ )
		memory_map[ f / STATIC_DWORD_SIZE_bit ] |= 1 << f % STATIC_DWORD_SIZE_bit;
}