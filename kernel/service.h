/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef KERNEL_SERVICE
	#define KERNEL_SERVICE

	void kernel_service_driver_mouse( struct LIB_SYS_STRUCTURE_MOUSE *mouse );
	uint64_t kernel_service_exec( const char *path, uint8_t stream );
	void kernel_service_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer );
	uintptr_t kernel_service_memory_alloc( uint64_t bytes );
	void kernel_service_memory_release( uintptr_t address, uint64_t bytes );
	void kernel_service_sleep( uint64_t ticks );
	void kernel_service_storage_read( struct LIB_SYS_STRUCTURE_STORAGE *storage );
	uint64_t kernel_service_uptime();
	uint64_t kernel_service_thread( uintptr_t function, uint8_t *string, uint8_t length );
#endif