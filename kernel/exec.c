/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

uint64_t kernel_exec( uint8_t *path, uint8_t length, uint8_t stream ) {
	// extract file name from command string
	uint64_t exec_length = lib_string_word( path, length );

	// get information about the file to run
	struct KERNEL_STORAGE_STRUCTURE_FILE kernel_exec_file = kernel -> storage_base_address[ kernel -> storage_root_id ].storage_file( kernel -> storage_root_id, path, exec_length );

	// if the file does not exist
	if( ! kernel_exec_file.id ) return EMPTY;

	// allocate work space for file
	uintptr_t kernel_exec_file_work_space;
	if( ! (kernel_exec_file_work_space = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel_exec_file.size_byte ) >> STATIC_PAGE_SIZE_shift )) ) return EMPTY;	// no enough memory space

	// load file into allocated space
	kernel -> storage_base_address[ kernel -> storage_root_id ].storage_read( kernel -> storage_root_id, kernel_exec_file.id, kernel_exec_file_work_space );

	// check if file contains proper ELF64 header

	// file have a proper ELF64 header?
	if( ! lib_elf_check( kernel_exec_file_work_space ) ) return EMPTY;

	// file's ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) kernel_exec_file_work_space;

	// it's an executable file?
	if( elf -> type != LIB_ELF_TYPE_executable ) { kernel_memory_release( kernel_exec_file_work_space, MACRO_PAGE_ALIGN_UP( kernel_exec_file.size_byte ) >> STATIC_PAGE_SIZE_shift ); return EMPTY; }	// no

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_table_position);

	// import libraries required by file
	kernel_library_import( elf );

	// create a new job in the queue
	struct KERNEL_TASK_STRUCTURE *child;
	if( ! (child = kernel_task_add( path, exec_length )) ) return EMPTY;	// failed

	// make space for the process paging table
	if( ! (child -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_mirror) ) return EMPTY;	// not enough memory space

	// describe the space under the process context stack
	if( ! kernel_page_alloc( (uintptr_t *) child -> cr3, KERNEL_CONTEXT_STACK_address, KERNEL_CONTEXT_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process ) ) return EMPTY;

	// put the information on how to start the process for the first time on the process context stack
	struct KERNEL_IDT_STRUCTURE_RETURN *stack = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) child -> cr3, KERNEL_CONTEXT_STACK_pointer - STATIC_PAGE_SIZE_byte ) + KERNEL_PAGE_mirror + (STATIC_PAGE_SIZE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	stack -> cs = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	stack -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	stack -> ss = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// the context stack top pointer
	child -> rsp = KERNEL_CONTEXT_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// set the process entry address
	stack -> rip = elf -> program_entry_position;

	//---------------

	// length of string to process
	uint64_t args = (length & ~STATIC_BYTE_HALF_mask) + 0x18;
	uint8_t *process_stack = (uint8_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( args ) >> STATIC_PAGE_SIZE_shift );

	// stack pointer of process
	stack -> rsp = KERNEL_EXEC_STACK_pointer - ((length & ~STATIC_BYTE_HALF_mask) + 0x18);

	// share to process:

	// length of args in Bytes
	uint64_t *arg_length = (uint64_t *) &process_stack[ MACRO_PAGE_ALIGN_UP( args ) - args ]; *arg_length = length;

	// and string itself
	for( uint64_t i = 0; i < length; i++ ) process_stack[ MACRO_PAGE_ALIGN_UP( args ) - args + 0x08 + i ] = path[ i ];

	// map stack space to process paging array
	kernel_page_map( (uintptr_t *) child -> cr3, (uintptr_t) process_stack, stack -> rsp & STATIC_PAGE_mask, MACRO_PAGE_ALIGN_UP( args ) >> STATIC_PAGE_SIZE_shift, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

	// process memory usage
	child -> page += MACRO_PAGE_ALIGN_UP( args ) >> STATIC_PAGE_SIZE_shift;

	// process stack size
	child -> stack += MACRO_PAGE_ALIGN_UP( args ) >> STATIC_PAGE_SIZE_shift;

	//---------------

	// calculate unpacked executable size in Pages
	uint64_t size_page = EMPTY;

	// prepare the memory space for segments used by the process
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
 		// ignore blank entries
 		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// set farthest point of loadable segment of library
		if( elf_header[ e ].type == LIB_ELF_HEADER_TYPE_load )
			size_page = (MACRO_PAGE_ALIGN_UP( elf_header[ e ].virtual_address + elf_header[ e ].segment_size ) - KERNEL_EXEC_BASE_address) >> STATIC_PAGE_SIZE_shift;
	}

	// allocate executable space
	uintptr_t kernel_exec_content = kernel_memory_alloc( size_page );

	// load program segments in place
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
		// ignore blank entries
		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// segment destination
		uint8_t *target = (uint8_t *) ((elf_header[ e ].virtual_address - KERNEL_EXEC_BASE_address) + kernel_exec_content);

		// segment source
		uint8_t *source = (uint8_t *) ((uintptr_t) elf + elf_header[ e ].segment_offset);

		// copy segment contents into place
		for( uint64_t j = 0; j < elf_header[ e ].segment_size; j++ ) target[ j ] = source[ j ];
	}

	// map executable space to its paging array
	kernel_page_map( (uintptr_t *) child -> cr3, kernel_exec_content, KERNEL_EXEC_BASE_address, size_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

	// process memory usage
	child -> page += size_page;

	// assign memory space for binary memory map with same size as kernels
	child -> memory_map = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STATIC_DIVIDE_BY_8_shift ) >> STATIC_PAGE_SIZE_shift );

	// fill in the binary memory map
	for( uint64_t i = 8; i < MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STATIC_DIVIDE_BY_8_shift ) >> STATIC_DIVIDE_BY_DWORD_shift; i++ ) child -> memory_map[ i ] = STATIC_MAX_unsigned;

	// mark as occupied pages used by the executable
	kernel_memory_acquire( child -> memory_map, size_page );

	// make a default input stream
	child -> stream_in = kernel_stream();

	// set default output stream based on flag
	struct KERNEL_TASK_STRUCTURE *parent = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();
	switch( stream ) {
		case LIB_SYS_STREAM_FLOW_out_to_parent_in: {
			// the child's output to the parent's input
			child -> stream_out = parent -> stream_in;

			// ready
			break;
		}
		case LIB_SYS_STREAM_FLOW_out_to_in: {
			// loop type stream
			child -> stream_out = child -> stream_in;
		
			// ready
			break;
		}
		default: {
			// inherit parent output
			child -> stream_out = parent -> stream_out;
		}
	}

	// if the output stream exists
	if( child -> stream_out ) child -> stream_out -> count++;	// increase its usage
	else
		// create new one
		child -> stream_out = kernel_stream();

	// map kernel space to process
	kernel_page_merge( (uint64_t *) kernel -> page_base_address, (uint64_t *) child -> cr3 );

	// link required functions of imported libraries
	kernel_exec_link( elf, kernel_exec_content );

	// release file content
	kernel_memory_release( kernel_exec_file_work_space, MACRO_PAGE_ALIGN_UP( kernel_exec_file.size_byte ) >> STATIC_PAGE_SIZE_shift );

	// process ready to run
	child -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_init;

	// return PID and pointer to created job
	return child -> pid;
}

void kernel_exec_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t executable_space ) {
	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uint64_t) elf + elf -> section_table_position);

	// retrieve information about:
	uintptr_t *got = EMPTY;	// global offset table
	uint8_t *strtab = EMPTY;	// function names
	struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *rela = EMPTY;	// dynamic relocation
	uint64_t rela_size_byte = EMPTY;
	struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *dynsym = EMPTY;	// dynamic symbols

	// check each entry of section
	for( uint16_t e = 0; e < elf -> section_entry_count; e++ ) {
		// function names?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_strtab )
			// first one only
			if( ! strtab ) strtab = (uint8_t *) ((uintptr_t) elf + elf_section[ e ].file_offset);
		
		// global offset table?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_progbits ) {
			got = (uintptr_t *) (elf_section[ e ].virtual_address - KERNEL_EXEC_BASE_address + executable_space) + 0x03;	// first 2 entries are not used by us
		}
		
		// dynamic relocations?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_rela ) {
			rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) ((uintptr_t) elf + elf_section[ e ].file_offset);
			rela_size_byte = elf_section[ e ].size_byte;
		}
		
		// dynamic symbols?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_dynsym )
			dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) ((uintptr_t) elf + elf_section[ e ].file_offset);
	}

	// external libraries are not required?
	if( ! rela ) return;	// yes

	// for each entry in dynamic relocation
	for( uint64_t i = 0; i < rela_size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION ); i++ ) {
		// it's a local function?
		if( dynsym[ rela[ i ].index ].address ) continue;	// leave it

		// update library function address
		got[ i ] = kernel_library_function( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ], lib_string_length( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ] ) );
	}
}