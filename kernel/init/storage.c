/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_storage() {
	// allocate space for the list of available media
	kernel -> storage_base_address = (struct KERNEL_STORAGE_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STORAGE_limit * sizeof( struct KERNEL_STORAGE_STRUCTURE ) ) >> STATIC_PAGE_SIZE_shift );

	// no main medium by default
	kernel -> storage_root_id = STATIC_MAX_unsigned;

	// check each attached module
	for( uint64_t i = 0; i < limine_module_request.response -> module_count; i++ )
		// module type of VFS?
		if( lib_vfs_check( (uintptr_t) limine_module_request.response -> modules[ i ] -> address, limine_module_request.response -> modules[ i ] -> size ) ) {
			// register the medium of type MEMORY and save
			struct KERNEL_STORAGE_STRUCTURE *storage = kernel_storage_register( KERNEL_STORAGE_TYPE_memory );

			// slot aquired?
			if( storage ) {
				// set device properties
				storage -> device_blocks = limine_module_request.response -> modules[ i ] -> size;
				storage -> device_first_block = (uint64_t) limine_module_request.response -> modules[ i ] -> address;
				storage -> storage_file = (void *) kernel_storage_file;
				storage -> storage_read = (void *) kernel_storage_read;

				// initialize VFS storage
				lib_vfs_init( (struct LIB_VFS_STRUCTURE *) storage -> device_first_block );
			}
		}

	// find media containing main executable file
	for( int64_t i = 0; i < KERNEL_STORAGE_limit; i++ ) {
		// get properties of file
		struct KERNEL_STORAGE_STRUCTURE_FILE file = kernel -> storage_base_address[ i ].storage_file( i, kernel_init_exec_file, lib_string_length( kernel_init_exec_file ) );

		// storage contains requested file?
		if( file.id ) {	// yes 
			// save storage ID
			kernel -> storage_root_id = i;

			// update kernel task with
			struct KERNEL_STRUCTURE_TASK *task = kernel_task_active();

			// main storage device
			task -> storage = i;
			// and current directory we are inside
			task -> directory = kernel -> storage_base_address[ i ].device_first_block;

			// show information about storage
			kernel_log( "System disk [KiB]: %d\n", MACRO_PAGE_ALIGN_UP( kernel -> storage_base_address[ i ].device_blocks ) >> STATIC_DIVIDE_BY_1024_shift );
		}
	}
}
