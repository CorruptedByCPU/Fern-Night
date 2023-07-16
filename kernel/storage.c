/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

struct KERNEL_STORAGE_STRUCTURE *kernel_storage_register( uint8_t type ) {
	// block access to binary memory map (only one at a time)
	while( __sync_val_compare_and_swap( &kernel -> storage_semaphore, UNLOCK, LOCK ) );

	// find an empty position
	for( uint8_t i = 0; i < KERNEL_STORAGE_limit; i++ )
		// free entry?
		if( ! kernel -> storage_base_address[ i ].device_type ) {
			// mark slot as used
			kernel -> storage_base_address[ i ].device_type = type;

			// unlock access to binary memory map
			__atomic_clear( &kernel -> storage_semaphore, __ATOMIC_RELEASE );

			// return pointer to device slot
			return (struct KERNEL_STORAGE_STRUCTURE *) &kernel -> storage_base_address[ i ];
		}

	// unlock access to binary memory map
	kernel -> storage_semaphore = UNLOCK;

	// no space available
	return EMPTY;
}

struct KERNEL_STORAGE_STRUCTURE_FILE kernel_storage_file( uint64_t storage, uint8_t *path, uint16_t length ) {
	// if storage type of MEMORY
	if( kernel -> storage_base_address -> device_type == KERNEL_STORAGE_TYPE_memory )
		// return properties of file
		return lib_vfs_file( (struct LIB_VFS_STRUCTURE *) kernel -> storage_base_address[ storage ].device_first_block, path, length );

	// file not found
	return (struct KERNEL_STORAGE_STRUCTURE_FILE) { EMPTY };
};

void kernel_storage_read( uint64_t storage, uint64_t id, uintptr_t destination ) {
	// storage type of KERNEL_STORAGE_TYPE_memory?
	if( kernel -> storage_base_address[ storage ].device_type == KERNEL_STORAGE_TYPE_memory ) lib_vfs_read( (struct LIB_VFS_STRUCTURE *) id, destination );
}