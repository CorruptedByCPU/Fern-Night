/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_library() {
	// prepare space for the stream space
	kernel -> library_base_address = (struct KERNEL_LIBRARY_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_LIBRARY_limit * sizeof( struct KERNEL_LIBRARY_STRUCTURE ) ) >> STATIC_PAGE_SIZE_shift );

	// assign space for memory map of library space
	kernel -> library_memory_map_address = (uint32_t *) (kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STATIC_DIVIDE_BY_8_shift ) >> STATIC_PAGE_SIZE_shift ) );

	// fill memory map with available pages
	for( uint64_t page = 0; page < KERNEL_EXEC_BASE_address >> STATIC_PAGE_SIZE_shift; page++ )
		kernel -> library_memory_map_address[ page / STATIC_DWORD_SIZE_bit ] |= 1 << page % STATIC_DWORD_SIZE_bit;
}