/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_service_driver_mouse( struct LIB_SYS_STRUCTURE_MOUSE *mouse ) {
	// return information about framebuffer width and height in pixels
	mouse -> x = kernel -> driver_ps2_mouse_x;
	mouse -> y = kernel -> driver_ps2_mouse_y;
	mouse -> x_absolute = kernel -> driver_ps2_mouse_x_absolute;
	mouse -> y_absolute = kernel -> driver_ps2_mouse_y_absolute;
	mouse -> status = kernel -> driver_ps2_mouse_status;
}

void kernel_service_exit( void ) {
	// properties of current task
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// mark task as closed and not active
	task -> flags |= KERNEL_TASK_FLAG_closed;
	task -> flags &= ~KERNEL_TASK_FLAG_active;

	// release rest AP time
	__asm__ volatile( "int $0x20" );
}

void kernel_service_memory( struct LIB_SYS_STRUCTURE_MEMORY *memory ) {
	// return information about

	// all available pages
	memory -> total = kernel -> page_total;

	// and currently free
	memory -> available = kernel -> page_available;
}

void kernel_service_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer ) {
	// find free space in process memory for framebuffer
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// return information about framebuffer width and height in pixels
	framebuffer -> width_pixel = kernel -> framebuffer_width_pixel;
	framebuffer -> height_pixel = kernel -> framebuffer_height_pixel;
	framebuffer -> scanline_byte = kernel -> framebuffer_scanline_byte;

	// if there is no "manager" of framebuffer, yet
	if( ! kernel -> framebuffer_manager )
		// and framebuffer space was successfully shared with process
		if( (framebuffer -> base_address = (uint32_t *) kernel_memory_share( (uintptr_t) kernel -> framebuffer_base_address - KERNEL_PAGE_mirror, kernel -> framebuffer_scanline_byte * kernel -> framebuffer_height_pixel )) )
			// assign process as manager of framebuffer
			kernel -> framebuffer_manager = task -> pid;

	// inform about framebuffer manager
	framebuffer -> pid = kernel -> framebuffer_manager;
}

void kernel_service_ipc_send( uint64_t pid, uint8_t *data ) {
	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> ipc_semaphore, UNLOCK, LOCK ) );

	// find free entry
	uint64_t i = EMPTY;
	while( TRUE ) {
		// free entry?
		if( kernel -> ipc_base_address[ i ].ttl < kernel -> hpet_microtime ) {
			// set the message aging time
			kernel -> ipc_base_address[ i ].ttl = kernel -> hpet_microtime + KERNEL_IPC_timeout;

			// set the PID of the process sending the message
			kernel -> ipc_base_address[ i ].source = kernel_task_pid();

			// set the PID of target process
			kernel -> ipc_base_address[ i ].target = pid;

			// load data into message
			for( uint8_t d = 0; d < LIB_SYS_IPC_DATA_size_byte; d++ )
				kernel -> ipc_base_address[ i ].data[ d ] = data[ d ];

			// message sent
			break;
		}

		// nope, start from begining if end of list
		if( ++i >= KERNEL_IPC_limit ) i = EMPTY;
	}

	// enable access to the message list
	kernel -> ipc_semaphore = UNLOCK;
}

uint8_t kernel_service_ipc_receive( struct LIB_SYS_STRUCTURE_IPC *message, uint8_t type ) {
	// by default no message for us
	uint8_t flag = FALSE;

	// get the ID of the calling process
	uint64_t target = kernel_task_pid();

	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> ipc_semaphore, UNLOCK, LOCK ) );

	// find message for us
	for( uint64_t i = 0; i < KERNEL_IPC_limit; i++ ) {
		// message available?
		if( kernel -> ipc_base_address[ i ].ttl > kernel -> hpet_microtime ) {
			// message properties
			struct LIB_SYS_STRUCTURE_IPC_DEFAULT *current = (struct LIB_SYS_STRUCTURE_IPC_DEFAULT *) kernel -> ipc_base_address[ i ].data;

			// requested message type?
			if( type && current -> type != type ) continue;	// no

			// message destined for the process
			if( kernel -> ipc_base_address[ i ].target == target ) {
				// move message to process environment
				uint8_t *from = (uint8_t *) &kernel -> ipc_base_address[ i ];
				uint8_t *to = (uint8_t *) message;
				for( uint8_t j = 0; j < sizeof( struct LIB_SYS_STRUCTURE_IPC ); j++ ) to[ j ] = from[ j ];

				// mark the entry as free
				kernel -> ipc_base_address[ i ].ttl = EMPTY;

				// message transferred
				flag = TRUE;

				// message sent to the process
				break;
			}

		}
	}

	// enable access to the message list
	kernel -> ipc_semaphore = UNLOCK;

	// return from routine with flag
	return flag;
}

uintptr_t kernel_service_memory_alloc( uint64_t bytes ) {
	// current task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// convert the size of the space in bytes into pages
	uint64_t pages = MACRO_PAGE_ALIGN_UP( bytes ) >> STATIC_PAGE_SIZE_shift;

	// acquire N continuous pages
	uintptr_t allocated = kernel_memory_acquire( task -> memory_map, pages ) << STATIC_PAGE_SIZE_shift;

	// if available
	if( allocated ) {
		// return the space address describing the found set of N bits
		if( ! kernel_page_alloc( (uintptr_t *) task -> cr3, allocated, pages, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) {
			// error
			kernel_log( "\nNo enough memory." ); while( TRUE );
		}

		// process memory usage
		task -> page += pages;

		// return the address of the first page in the collection
		return allocated;
	}

	// no free space
	return EMPTY;
}

void kernel_service_memory_release( uintptr_t address, uint64_t bytes ) {
	// current task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// convert the size of the space in bytes into pages
	uint64_t pages = MACRO_PAGE_ALIGN_UP( bytes ) >> STATIC_PAGE_SIZE_shift;

	// release occupied pages in binary memory map
	for( uint64_t i = (address & ~KERNEL_PAGE_mirror) >> STATIC_PAGE_SIZE_shift; i < ((address & ~KERNEL_PAGE_mirror) >> STATIC_PAGE_SIZE_shift) + pages; i++ ) {
		// remove page from paging structure
		uintptr_t page = kernel_page_remove( (uintptr_t *) task -> cr3, i << STATIC_PAGE_SIZE_shift );

		// if released
		if( page ) {
			// return page back to stack
			kernel_memory_release_page( page );

			// process memory usage
			task -> page--;
		}

		// release page in binary memory map of process
		kernel_memory_dispose( task -> memory_map, i, 1 );
	}
}

uintptr_t kernel_service_memory_share( uintptr_t source, uint64_t bytes, uint64_t pid ) {
	// convert the size of the space in bytes into pages
	uint64_t pages = MACRO_PAGE_ALIGN_UP( bytes ) >> STATIC_PAGE_SIZE_shift;

	// properties of target task
	struct KERNEL_TASK_STRUCTURE *target = (struct KERNEL_TASK_STRUCTURE *) kernel_task_by_id( pid );

	// acquire space from target task
	uint64_t address = kernel_memory_acquire( target -> memory_map, pages ) << STATIC_PAGE_SIZE_shift;

	// connect memory space of parent process with child
	kernel_page_clang( (uintptr_t *) target -> cr3, source, address, pages, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process | KERNEL_PAGE_FLAG_shared );

	// return the address of the first page in the collection
	return address;
}

void kernel_service_sleep( uint64_t ms ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// go to sleep for N ms
	task -> sleep = kernel -> hpet_microtime + 1;

	// release the remaining CPU time
	__asm__ volatile( "int $0x20" );
}

void kernel_service_storage_read( struct LIB_SYS_STRUCTURE_STORAGE *storage ) {
	// get properties of file
	struct KERNEL_STORAGE_STRUCTURE_FILE file = kernel -> storage_base_address[ kernel -> storage_root_id ].storage_file( kernel -> storage_root_id, storage -> name, storage -> length );

	// system storage contains requested file?
	if( file.id ) {	// yes
		// allocate space for file
		if( ! (storage -> address = kernel_service_memory_alloc( file.size_byte )) ) return;

		// load file into allocated space
		kernel -> storage_base_address[ kernel -> storage_root_id ].storage_read( kernel -> storage_root_id, file.id, storage -> address );

		// return information about file size in Bytes
		storage -> size_byte = file.size_byte;
	}
}

uint16_t kernel_service_task_status( uint64_t pid ) {
	// retrieve task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_by_id( pid );

	// if task doesn't exist
	if( ! task ) return EMPTY;

	// return task status
	return task -> flags;
}

uint64_t kernel_service_uptime() {
	// return ticks passed from kernel environment setup
	return kernel -> hpet_microtime;
}

uintptr_t kernel_service_task() {
	// deny access to modification of job queue
	while( __sync_val_compare_and_swap( &kernel -> task_queue_semaphore, UNLOCK, LOCK ) );

	// assign place for task descriptor list
	uintptr_t *address = (uintptr_t *) kernel_service_memory_alloc( (sizeof( struct LIB_SYS_STRUCTURE_TASK ) * kernel -> task_count) + (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) );

	// register area size
	*address = MACRO_PAGE_ALIGN_UP( (sizeof( struct LIB_SYS_STRUCTURE_TASK ) * kernel -> task_count) + (STATIC_PTR_SIZE_byte << STATIC_MULTIPLE_BY_2_shift) ) >> STATIC_PAGE_SIZE_shift;

	// find an free entry
	struct LIB_SYS_STRUCTURE_TASK *task = (struct LIB_SYS_STRUCTURE_TASK *) &address[ 2 ];
	for( uint64_t t = 1; t < (KERNEL_TASK_TABLE_LENGTH_page << STATIC_PAGE_SIZE_shift) / sizeof( struct KERNEL_TASK_STRUCTURE ); t++ ) {
		// entry used?
		if( ! kernel -> task_queue_address[ t ].flags ) continue;	// no

		// do not pass kernel entry
		if( ! kernel -> task_queue_address[ t ].pid ) continue;

		// share default information about task
		task -> pid = kernel -> task_queue_address[ t ].pid;
		task -> pid_parent = kernel -> task_queue_address[ t ].pid_parent;
		task -> sleep = kernel -> task_queue_address[ t ].sleep;
		task -> page = kernel -> task_queue_address[ t ].page;
		task -> flags = kernel -> task_queue_address[ t ].flags;

		// task name and length
		task -> length = kernel -> task_queue_address[ t ].length;
		for( uint8_t n = 0; n < task -> length; n++ ) task -> name[ n ] = kernel -> task_queue_address[ t ].name[ n ];

		// descriptor ready, next one
		task++;
	}

	// last entry set as empty
	task -> pid = EMPTY;

	// free access to job queue
	kernel -> task_queue_semaphore = UNLOCK;

	// return pointer to task list
	return (uintptr_t) &address[ 2 ];
}

uint64_t kernel_service_thread( uintptr_t function, uint8_t *string, uint8_t length ) {
	// create a new thread in queue
	struct KERNEL_TASK_STRUCTURE *thread;
	if( ! (thread = kernel_task_add( string, length )) ) return EMPTY;	// failed

	// make space for thread paging table
	if( ! (thread -> cr3 = kernel_memory_alloc_page() | KERNEL_PAGE_mirror) ) return EMPTY;	// not enough memory space

	// describe space under thread context stack
	if( ! kernel_page_alloc( (uintptr_t *) thread -> cr3, KERNEL_CONTEXT_STACK_address, KERNEL_CONTEXT_STACK_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_process ) ) return EMPTY;

	// put the information on how to start the process for the first time on the process context stack
	struct KERNEL_IDT_STRUCTURE_RETURN *stack = (struct KERNEL_IDT_STRUCTURE_RETURN *) (kernel_page_address( (uintptr_t *) thread -> cr3, KERNEL_CONTEXT_STACK_pointer - STATIC_PAGE_SIZE_byte ) + KERNEL_PAGE_mirror + (STATIC_PAGE_SIZE_byte - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN )));

	// code descriptor
	stack -> cs = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, cs_ring3 ) | 0x03;

	// basic processor state flags
	stack -> eflags = KERNEL_TASK_EFLAGS_default;

	// stack descriptor
	stack -> ss = offsetof( struct KERNEL_INIT_GDT_STRUCTURE, ds_ring3 ) | 0x03;

	// context stack top pointer
	stack -> rsp = KERNEL_EXEC_STACK_pointer - 0x10;	// no args

	// set thread entry address
	stack -> rip = function;

	// describe space under thread stack
	if( ! kernel_page_alloc( (uintptr_t *) thread -> cr3, KERNEL_EXEC_STACK_address, KERNEL_EXEC_STACK_SIZE_page, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process ) ) return EMPTY;

	// stack pointer of process
	thread -> rsp = KERNEL_CONTEXT_STACK_pointer - sizeof( struct KERNEL_IDT_STRUCTURE_RETURN );

	// thread memory usage
	thread -> page += KERNEL_EXEC_STACK_SIZE_page;

	// thread stack size
	thread -> stack += KERNEL_EXEC_STACK_SIZE_page;

	// aquire parent task properties
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// threads use same memory map as parent
	thread -> memory_map = task -> memory_map;

	// threads use same streams as parent
	thread -> stream_in = task -> stream_in; task -> stream_in -> count++;
	thread -> stream_out = task -> stream_out; task -> stream_out -> count++;

	// map parent space to thread
	kernel_page_merge( (uint64_t *) task -> cr3, (uint64_t *) thread -> cr3 );

	// thread ready to run
	thread -> flags |= KERNEL_TASK_FLAG_active | KERNEL_TASK_FLAG_thread | KERNEL_TASK_FLAG_init;

	// return process ID of new thread
	return thread -> pid;
}