/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_memory() {
	// limine shared with us a memory map?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {
		// show error
		kernel_log( "Houston, we have a problem." );

		// hold the door
		while( TRUE );
	}

	// force consistency of available memory space for use (clean it up)
	uint64_t largest_continous_space_in_bytes = EMPTY;
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// USABLE memory space?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
			// clean it up
			kernel_page_clean( limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> length >> STATIC_PAGE_SIZE_shift );

			// inside largest continous memory space we will create kernel environment metadata and a binary memory map next to it

			// this space is larger than previous one?
			if( largest_continous_space_in_bytes < limine_memmap_request.response -> entries[ i ] -> length ) {
				// keep logical address of largest continous memory space (reflection in Higher Half)
				kernel = (struct KERNEL_STRUCTURE *) (limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror);

				// keep size information
				largest_continous_space_in_bytes = limine_memmap_request.response -> entries[ i ] -> length;
			}
		}
	}

	// binary memory map base address after kernel environment variables/functions/rountines
	kernel -> memory_base_address = (uint32_t *) (MACRO_PAGE_ALIGN_UP( (uintptr_t) kernel + sizeof( struct KERNEL_STRUCTURE ) ));

	// describe all spaces marked as USBALE in binary memory map
	uintptr_t binary_memory_map_limit = EMPTY;
	uint64_t amount_of_available_pages = EMPTY;
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ ) {
		// we need to know farthest fragment of memory space
	
		// USABLE or BOOTLOADER_RECLAIMABLE memory space?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ) {
			// keep farthest part of RAM memory
			if( binary_memory_map_limit < limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length ) binary_memory_map_limit = limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length;

			// keep number of pages registered in the binary memory map
			amount_of_available_pages += limine_memmap_request.response -> entries[ i ] -> length >> STATIC_PAGE_SIZE_shift;

			// also, we register only space guaranteed to be available for use
			// USABLE memory space?
			if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE ) {
				// register pages that are part of USABLE memory space
				for( uint64_t page = limine_memmap_request.response -> entries[ i ] -> base >> STATIC_PAGE_SIZE_shift; page < (limine_memmap_request.response -> entries[ i ] -> base + limine_memmap_request.response -> entries[ i ] -> length) >> STATIC_PAGE_SIZE_shift; page++ )
					kernel -> memory_base_address[ page / STATIC_DWORD_SIZE_bit ] |= 1 << page % STATIC_DWORD_SIZE_bit;
			}

			// why we do not map BOOTLOADER_RECLAIMABLE yet?
			// although this is unlikely, we do not want to accidentally destroy data (e.g. stack, paging tables - which we are using right now)
		}
	}

	// calculate size of binary memory map in pages based on farthest end of memory map entry
	uint64_t initial_structure_length = MACRO_PAGE_ALIGN_UP( (binary_memory_map_limit >> STATIC_PAGE_SIZE_shift) >> STATIC_DIVIDE_BY_8_shift ) >> STATIC_PAGE_SIZE_shift;
	// add size of kernel environment space in pages
	initial_structure_length += MACRO_PAGE_ALIGN_UP( sizeof( struct KERNEL_STRUCTURE ) ) >> STATIC_PAGE_SIZE_shift;

	// first N pages of binary memory map used by kernel environment variables/functions and binary memory map itself, mark as unavailable (used)
	for( uint64_t p = ((uintptr_t) kernel & ~KERNEL_PAGE_mirror) >> STATIC_PAGE_SIZE_shift; p < (((uintptr_t) kernel & ~KERNEL_PAGE_mirror) >> STATIC_PAGE_SIZE_shift) + initial_structure_length; p++ )
		kernel -> memory_base_address[ p / STATIC_DWORD_SIZE_bit ] &= ~(1 << p % STATIC_DWORD_SIZE_bit);

	// specification of binary memory map
	kernel -> page_total = amount_of_available_pages;
	kernel -> page_available = amount_of_available_pages - initial_structure_length;
	kernel -> page_limit = binary_memory_map_limit >> STATIC_PAGE_SIZE_shift;
}
