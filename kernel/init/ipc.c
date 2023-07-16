/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_ipc() {
	// prepare the space for the stream space
	kernel -> ipc_base_address = (struct LIB_SYS_STRUCTURE_IPC *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_IPC_limit * sizeof( struct LIB_SYS_STRUCTURE_IPC ) ) >> STATIC_PAGE_SIZE_shift );
}