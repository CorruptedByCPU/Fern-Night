/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kernel_init_daemon_file[] = "gc.d";

void kernel_init_daemon() {
	// get information about the file to run
	struct KERNEL_STORAGE_STRUCTURE_FILE kernel_daemon_file = kernel -> storage_base_address[ kernel -> storage_root_id ].storage_file( kernel -> storage_root_id, kernel_init_daemon_file, sizeof( kernel_init_daemon_file ) - 1 );

	// if the file does not exist
	if( ! kernel_daemon_file.id ) return;

	// allocate work space for file
	uintptr_t kernel_daemon_file_work_space;
	if( ! (kernel_daemon_file_work_space = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel_daemon_file.size_byte ) >> STATIC_PAGE_SIZE_shift )) ) return;	// no enough memory space

	// load file into allocated space
	kernel -> storage_base_address[ kernel -> storage_root_id ].storage_read( kernel -> storage_root_id, kernel_daemon_file.id, kernel_daemon_file_work_space );

	// check if file contains proper ELF64 header

	// file have a proper ELF64 header?
	if( ! lib_elf_identify( kernel_daemon_file_work_space ) ) return;

	// file's ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) kernel_daemon_file_work_space;

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_table_position);

	// create a new job in the queue
	struct KERNEL_TASK_STRUCTURE *child;
	if( ! (child = kernel_task_add( kernel_init_daemon_file, sizeof( kernel_init_daemon_file ) - 1 )) ) return;	// failed

	// make space for the process paging table
	if( ! (child -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_mirror) ) return;	// not enough memory space

	// describe the space under the process context stack
	if( ! kernel_page_alloc( (uintptr_t *) child -> cr3, KERNEL_CONTEXT_STACK_address, KERNEL_CONTEXT_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process ) ) return;

	// process memory usage
	child -> page += KERNEL_CONTEXT_STACK_page;

	// put the information on how to start the process for the first time on the process context stack
	struct KERNEL_IDT_STRUCTURE_RETURN *stack = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) child -> cr3, KERNEL_CONTEXT_STACK_pointer - STATIC_PAGE_SIZE_byte ) + KERNEL_PAGE_mirror + (STATIC_PAGE_SIZE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	stack -> cs = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, cs_ring0 );
	// basic processor state flags
	stack -> eflags = KERNEL_TASK_EFLAGS_default;
	// current top-of-stack pointer
	stack -> rsp = KERNEL_CONTEXT_STACK_pointer;
	// stack descriptor
	stack -> ss = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, ds_ring0 );

	// the context stack top pointer
	child -> rsp = KERNEL_CONTEXT_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// calculate unpacked executable size in Pages
	uint64_t size_page = EMPTY;

	// prepare the memory space for segments used by the process
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
 		// ignore blank entries
 		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// set farthest point of loadable segment of library
		if( elf_header[ e ].type == LIB_ELF_HEADER_TYPE_load )
			size_page = MACRO_PAGE_ALIGN_UP( elf_header[ e ].virtual_address + elf_header[ e ].segment_size ) >> STATIC_PAGE_SIZE_shift;
	}

	// allocate executable space
	uintptr_t kernel_daemon_content = kernel_memory_alloc( size_page );

	// set the process entry address
	stack -> rip = kernel_daemon_content + elf -> program_entry_position;

	// load program segments in place
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
		// ignore blank entries
		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// segment destination
		uint8_t *target = (uint8_t *) (elf_header[ e ].virtual_address + kernel_daemon_content);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_header[ e ].segment_offset);

		// copy segment contents into place
		for( uint64_t j = 0; j < elf_header[ e ].segment_size; j++ ) target[ j ] = source[ j ];
	}

	// process memory usage
	child -> page += size_page;

	// make a default input stream
	child -> stream_in = kernel_stream();

	// process memory usage
	child -> page++;

	// set default output stream
	struct KERNEL_TASK_STRUCTURE *parent = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();
	child -> stream_out = parent -> stream_out;
	child -> stream_out -> count++;

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) child -> cr3 );

	// process ready to run
	child -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_daemon | KERNEL_TASK_FLAG_init;
}
