/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_free() {
	// request space for our array
	uint64_t free_page = MACRO_PAGE_ALIGN_UP( (limine_memmap_request.response -> entry_count + 1) << STATIC_MULTIPLE_BY_16_shift ) >> STATIC_PAGE_SIZE_shift;
	struct KERNEL_INIT_STRUCTURE_FREE *free = (struct KERNEL_INIT_STRUCTURE_FREE *) kernel_memory_alloc( free_page );

	// register all LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE inside array
	uint64_t free_count = EMPTY;	// amount if registered entries
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ )
		// BOOTLOADER_RECLAIMABLE memory space?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ) {
			// add to array
			free[ free_count ].size_page = limine_memmap_request.response -> entries[ i ] -> length >> STATIC_PAGE_SIZE_shift;
			free[ free_count++ ].base_address = MACRO_PAGE_ALIGN_DOWN( limine_memmap_request.response -> entries[ i ] -> base );
		}

	//----------------------------------------------------------------------
	// at this point, we CANNOT use any Limine structure anymore!
	// thats why we created array of RECLAIMABLE areas on stack...
	//----------------------------------------------------------------------

	// amount of freed up pages
	uint64_t freed_up = EMPTY;

	// force consistency of BOOTLOADER_RECLAIMABLE memory space (clean it up)
	for( uint64_t i = 0; i < free_count; i++ ) {
		// clean it up
		kernel_page_clean( free[ i ].base_address | KERNEL_PAGE_mirror, free[ i ].size_page );

		// register pages that are part of USABLE memory space
		for( uint64_t page = free[ i ].base_address >> STATIC_PAGE_SIZE_shift; page < (free[ i ].base_address >> STATIC_PAGE_SIZE_shift) + free[ i ].size_page; page++ )
			kernel -> memory_base_address[ page / STATIC_DWORD_SIZE_bit ] |= 1 << page % STATIC_DWORD_SIZE_bit;

		// available
		freed_up += free[ i ].size_page;
	}

	// release allocated array
	kernel_memory_release( (uintptr_t) free, free_page );

	// show information about increased memory
	kernel_log( "+%d KiB released.\n", freed_up << STATIC_MULTIPLE_BY_4_shift );
}