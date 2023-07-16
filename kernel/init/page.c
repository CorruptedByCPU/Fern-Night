/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_page() {
	// new kernel PML4 array
	kernel -> page_base_address = (uint64_t *) (kernel_memory_alloc_page() | KERNEL_PAGE_mirror);

	// map all usable memory space marked as USABLE, KERNEL_AND_MODULES, FRAMEBUFFER, BOOTLOADER_RECLAIMABLE
	for( uint64_t i = 0; i < limine_memmap_request.response -> entry_count; i++ )
		// USABLE, KERNEL_AND_MODULES, FRAMEBUFFER, BOOTLOADER_RECLAIMABLE memory space?
		if( limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_USABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_KERNEL_AND_MODULES || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || limine_memmap_request.response -> entries[ i ] -> type == LIMINE_MEMMAP_FRAMEBUFFER )
			// map memory space to kernel paging arrays
			kernel_page_map( kernel -> page_base_address, limine_memmap_request.response -> entries[ i ] -> base, limine_memmap_request.response -> entries[ i ] -> base | KERNEL_PAGE_mirror, MACRO_PAGE_ALIGN_UP( limine_memmap_request.response -> entries[ i ] -> length ) >> STATIC_PAGE_SIZE_shift, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// kernel file properties
	struct limine_file *limine_file = limine_kernel_file_request.response -> kernel_file;

	// kernel file ELF64 properties
	struct LIB_ELF_STRUCTURE *limine_file_elf64_header = (struct LIB_ELF_STRUCTURE *) limine_file -> address;

	// kernel file ELF64 header properties
	struct LIB_ELF_STRUCTURE_HEADER *limine_file_elf64_header_entry = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) limine_file_elf64_header + limine_file_elf64_header -> header_table_position);

	// retrieve flags and map kernel file segments in proper place
	for( uint16_t i = 0; i < limine_file_elf64_header -> header_entry_count; i++ ) {
		// ignore blank entries
		if( ! limine_file_elf64_header_entry[ i ].type || ! limine_file_elf64_header_entry[ i ].memory_size ) continue;

		// segment properties
		uintptr_t segment_offset = MACRO_PAGE_ALIGN_DOWN( limine_file_elf64_header_entry[ i ].virtual_address ) - 0xFFFFFFFF80000000;
		uint64_t segment_length = (MACRO_PAGE_ALIGN_UP( limine_file_elf64_header_entry[ i ].virtual_address + limine_file_elf64_header_entry[ i ].memory_size ) - 0xFFFFFFFF80000000) >> STATIC_PAGE_SIZE_shift;

		// default flag, for every segment
		uint16_t segment_flags = KERNEL_PAGE_FLAG_present;

		// update with additional flag (if exist)
		if( limine_file_elf64_header_entry[ i ].flags & LIB_ELF_FLAG_write ) segment_flags |= KERNEL_PAGE_FLAG_write;

		// map kernel space memory
		kernel_page_map( kernel -> page_base_address, limine_kernel_address_request.response -> physical_base + segment_offset, 0xFFFFFFFF80000000 + segment_offset, segment_length, segment_flags );
	}

	// create kernel stack space
	kernel_page_alloc( (uint64_t *) kernel -> page_base_address, KERNEL_CONTEXT_STACK_address, KERNEL_CONTEXT_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// register LAPIC controller space
	kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> lapic_base_address - KERNEL_PAGE_mirror, (uintptr_t) kernel -> lapic_base_address, STATIC_PAGE_SIZE_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// register I/O APIC controller space
	kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> io_apic_base_address - KERNEL_PAGE_mirror, (uintptr_t) kernel -> io_apic_base_address, STATIC_PAGE_SIZE_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );

	// register HPET controller space
	kernel_page_map( kernel -> page_base_address, (uintptr_t) kernel -> hpet_base_address - KERNEL_PAGE_mirror, (uintptr_t) kernel -> hpet_base_address, STATIC_PAGE_SIZE_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );
}