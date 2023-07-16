/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

struct KERNEL_LIBRARY_STRUCTURE *kernel_library_add() {
	// search for empty entry
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ )
		// found?
		if( ! kernel -> library_base_address[ i ].flags ) {
			// mark entry as reserved
			kernel -> library_base_address[ i ].flags |= KERNEL_LIBRARY_FLAG_reserved;
		
			// return pointer to entry
			return (struct KERNEL_LIBRARY_STRUCTURE *) &kernel -> library_base_address[ i ];
		}
	
	// nope
	return EMPTY;
}

struct KERNEL_LIBRARY_STRUCTURE *kernel_library_find( uint8_t *string, uint8_t length ) {
	// search for not active entry
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ )
		// library name with exact length and name?
		if( (kernel -> library_base_address[ i ].flags & KERNEL_LIBRARY_FLAG_active) && kernel -> library_base_address[ i ].length == length && lib_string_compare( string, (uint8_t *) &kernel -> library_base_address[ i ].name, length ) )
			// yes
			return (struct KERNEL_LIBRARY_STRUCTURE *) &kernel -> library_base_address[ i ];
	
	// nope
	return EMPTY;
}

uintptr_t kernel_library_function( uint8_t *string, uint64_t length ) {
	// search in every loaded library
	for( uint64_t i = 0; i < KERNEL_LIBRARY_limit; i++ ) {
		// entry exist?
		if( ! (kernel -> library_base_address[ i ].flags & KERNEL_LIBRARY_FLAG_active) ) continue;	// no

		// search thru available dynamic symbols inside library
		for( uint64_t j = 0; j < kernel -> library_base_address[ i ].dynsym_limit / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL ); j++ )
			// function we are looking for?
			if( lib_string_length( (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynsym[ j ].name_offset ] ) == length && lib_string_compare( string, (uint8_t *) &kernel -> library_base_address[ i ].strtab[ kernel -> library_base_address[ i ].dynsym[ j ].name_offset ], length ) )
				// yes
				return (uintptr_t) (kernel -> library_base_address[ i ].address + kernel -> library_base_address[ i ].dynsym[ j ].address);
	}

	// not found
	return EMPTY;
}

void kernel_library_import( struct LIB_ELF_STRUCTURE *elf ) {
	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uint64_t) elf + elf -> section_table_position);

	// by default we don't need any external libraries
	uint8_t *strtab = EMPTY;
	struct LIB_ELF_STRUCTURE_SECTION_DYNAMIC *dynamic = EMPTY;

	// retrieve information about dynamic and string table
	for( uint16_t e = 0; e < elf -> section_entry_count; e++ ) {
		// string table (only first one)
		if( ! strtab ) if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_strtab ) strtab = (uint8_t *) ((uint64_t) elf + elf_section[ e ].file_offset);

		// dynamic section
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_dynamic ) dynamic = (struct LIB_ELF_STRUCTURE_SECTION_DYNAMIC *) ((uintptr_t) elf + elf_section[ e ].file_offset);
	}

	// if dynamic section doesn't exist
	if( ! dynamic ) return;	// end of routine

	// load needed libraries
	while( dynamic -> type == LIB_ELF_SECTION_DYNAMIC_TYPE_needed ) { kernel_library_load( (uint8_t *) &strtab[ dynamic -> offset ], lib_string_length( (uint8_t *) &strtab[ dynamic -> offset ] ) ); dynamic++; }
}

void kernel_library_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t library_space ) {
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
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_progbits )
			got = (uintptr_t *) (elf_section[ e ].virtual_address + library_space) + 0x03;	// first 2 entries are not used by us
		
		// dynamic relocations?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_rela ) {
			rela = (struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION *) ((uintptr_t) elf + elf_section[ e ].file_offset);
			rela_size_byte = elf_section[ e ].size_byte;
		}
		
		// dynamic symbols?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_dynsym )
			dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (elf_section[ e ].virtual_address + library_space);
	}

	// external libraries are not required?
	if( ! rela ) return;	// yes

	// for each entry in dynamic relocation
	for( uint64_t i = 0; i < rela_size_byte / sizeof( struct LIB_ELF_STRUCTURE_DYNAMIC_RELOCATION ); i++ ) {
		// it's a local function?
		if( dynsym[ rela[ i ].index ].address ) {
			// insert corrected address of local function
			got[ i ] = dynsym[ rela[ i ].index ].address + library_space;

			// next relocation
			continue;
		}

		// update library function address
		got[ i ] = kernel_library_function( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ], lib_string_length( (uint8_t *) &strtab[ dynsym[ rela[ i ].index ].name_offset ] ) );
	}
}

struct KERNEL_LIBRARY_STRUCTURE *kernel_library_load( uint8_t *path, uint8_t length ) {
	// library already exist?
	struct KERNEL_LIBRARY_STRUCTURE *library = kernel_library_find( path, length );
	if( library ) return library;	// yes

	// prepare new entry for library
	library = kernel_library_add();

	// get information about library to load
	struct KERNEL_STORAGE_STRUCTURE_FILE file = kernel -> storage_base_address[ kernel -> storage_root_id ].storage_file( kernel -> storage_root_id, path, length );

	// allocate work space for file
	uintptr_t file_content = kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( file.size_byte ) >> STATIC_PAGE_SIZE_shift );

	// load file into allocated space
	kernel -> storage_base_address[ kernel -> storage_root_id ].storage_read( kernel -> storage_root_id, file.id, file_content );

	// check if file contains proper ELF64 header

	// file's ELF structure properties
	struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) file_content;

	// ELF header properties
	struct LIB_ELF_STRUCTURE_HEADER *elf_header = (struct LIB_ELF_STRUCTURE_HEADER *) ((uint64_t) elf + elf -> header_table_position);

	// ELF section properties
	struct LIB_ELF_STRUCTURE_SECTION *elf_section = (struct LIB_ELF_STRUCTURE_SECTION *) ((uint64_t) elf + elf -> section_table_position);

	// import libraries required by file
	kernel_library_import( elf );

	// calculate library size in Pages
	uint16_t size_page = EMPTY;

	// prepare the memory space for segments used by the process
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
 		// ignore blank entries
 		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// set farthest point of loadable segment of library
		if( elf_header[ e ].type == LIB_ELF_HEADER_TYPE_load )
			size_page = MACRO_PAGE_ALIGN_UP( elf_header[ e ].virtual_address + elf_header[ e ].segment_size ) >> STATIC_PAGE_SIZE_shift;
	}

	// acquire memory space inside library environment
	uintptr_t library_content = (kernel_memory_acquire( kernel -> library_memory_map_address, size_page ) << STATIC_PAGE_SIZE_shift) + KERNEL_LIBRARY_BASE_address;

	// map aquired memory space
	kernel_page_alloc( kernel -> page_base_address, library_content, size_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_library );

	// preserve information in library entry
	library -> address = library_content;
	library -> size_page = size_page;

	// prepare the memory space of segments used by the process
	for( uint16_t e = 0; e < elf -> header_entry_count; e++ ) {
		// ignore blank entries
		if( ! elf_header[ e ].type || ! elf_header[ e ].memory_size ) continue;

		// only loadable segmnets
		if( elf_header[ e ].type != LIB_ELF_HEADER_TYPE_load ) continue;

		// copy segment contents into place
		uint8_t *source = (uint8_t *) file_content + elf_header[ e ].segment_offset;
		uint8_t *target = (uint8_t *) library_content + elf_header[ e ].virtual_address;
		for( uint64_t j = 0; j < elf_header[ e ].segment_size; j++ ) target[ j ] = source[ j ];
	}

	// retrieve information about symbol and string (table)
	for( uint16_t e = 0; e < elf -> section_entry_count; e++ ) {
		// string table?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_strtab ) {
			// preserve only first one
			if( ! library -> strtab ) library -> strtab = (uint8_t *) (library_content + elf_section[ e ].virtual_address);
		}

		// symbol table?
		if( elf_section[ e ].type == LIB_ELF_SECTION_TYPE_dynsym ) {
			// preserve its address
			library -> dynsym = (struct LIB_ELF_STRUCTURE_DYNAMIC_SYMBOL *) (library_content + elf_section[ e ].virtual_address);
		
			// and size
			library -> dynsym_limit = elf_section[ e ].size_byte;
		}
	}

	// link required functions of imported libraries
	kernel_library_link( elf, library_content );

	// share access to library content space for processes (read-only)
	kernel_page_flags( kernel -> page_base_address, library_content, size_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_library );

	// update library with name
	library -> length = length;
	for( uint8_t i = 0; i < length; i++ ) library -> name[ i ] = path[ i ];

	// library parsed
	library -> flags |= KERNEL_LIBRARY_FLAG_active;

	// library loaded, return its pointer
	return library;
}
