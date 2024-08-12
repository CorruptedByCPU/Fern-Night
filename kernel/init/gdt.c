/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_gdt() {
	// prepare space for Global Descriptor Table
	kernel_gdt_base_address = (struct KERNEL_INIT_GDT_STRUCTURE_ENTRY *) (kernel_memory_alloc_page() | KERNEL_PAGE_mirror);

	// create code descriptor ring0 (CS)
	kernel_gdt_base_address[ 1 ].access = 0x98;
	kernel_gdt_base_address[ 1 ].flags_and_limit_high = 0x20;
	// create data descriptor ring0 (DS)
	kernel_gdt_base_address[ 2 ].access = 0x92;
	kernel_gdt_base_address[ 2 ].flags_and_limit_high = 0x20;
	// create data descriptor ring3 (DS)
	kernel_gdt_base_address[ 4 ].access = 0xF2;
	kernel_gdt_base_address[ 4 ].flags_and_limit_high = 0x20;
	// create code descriptor ring3 (CS)
	kernel_gdt_base_address[ 5 ].access = 0xF8;
	kernel_gdt_base_address[ 5 ].flags_and_limit_high = 0x20;

	// configure header of Global Descriptor Table
	kernel_gdt_header.size = STATIC_PAGE_SIZE_byte;
	kernel_gdt_header.address = (uintptr_t) kernel_gdt_base_address;

	// load a new Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel_gdt_header) );

	// set valid descriptor values
	kernel_gdt_descriptors();

	// Task State Segment Table
	kernel_tss_table.rsp0 = KERNEL_CONTEXT_STACK_pointer;
}
