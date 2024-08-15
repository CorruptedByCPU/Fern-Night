/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_task( void ) {
	// from list of active tasks on individual logical processors
	// select currently processed position relative to current logical processor
	struct KERNEL_STRUCTURE_TASK *task = (struct KERNEL_STRUCTURE_TASK *) kernel -> task_cpu_address[ kernel_lapic_id() ];

	// keep current top of stack pointer
	__asm__ volatile( "mov %%rsp, %0" : "=rm" (task -> rsp) );

	// and task is free to be executed by another CPU
	task -> flags &= ~KERNEL_TASK_FLAG_exec;

	// find next task to be processed
	task = kernel_task_select( (((uint64_t) task - (uint64_t) kernel -> task_base_address) / sizeof( struct KERNEL_STRUCTURE_TASK )) + 1 );

	// save task ID in the active tasks list for current logical processor
	kernel -> task_cpu_address[ kernel_lapic_id() ] = (uintptr_t) task;

	// restore top of the stack pointer
	__asm__ volatile( "movq %0, %%rsp" : "=rm" ( task -> rsp ) );

	// reload paging tables into the process space
	__asm__ volatile( "mov %0, %%cr3" ::"r" (task -> cr3 - KERNEL_PAGE_mirror) );

	// reload CPU cycle counter in APIC controller
	kernel_lapic_reload();

	// accept current interrupt call
	kernel_lapic_accept();

	// first run of the task?
	if( task -> flags & KERNEL_TASK_FLAG_init ) {
		// disable init flag
		task -> flags &= ~KERNEL_TASK_FLAG_init;

		// if daemon, pass a pointer to the kernel environment specification
		if( task -> flags & KERNEL_TASK_FLAG_module ) __asm__ volatile( "" :: "D" (kernel), "S" (EMPTY) );
		else {
			// retrieve from stack
			uint64_t *arg = (uint64_t *) (task -> rsp + offsetof( struct KERNEL_IDT_STRUCTURE_RETURN, rsp ) );
			uint64_t *argc = (uint64_t *) *arg;

			// length of string
			__asm__ volatile( "" :: "D" (*argc) );
			
			// pointer to string
			__asm__ volatile( "" :: "S" (*arg + 0x08) );
		}

		// reset FPU state
		__asm__ volatile( "fninit" );

		// kernel guarantee clean registers at first run
		__asm__ volatile( "xor %r15, %r15\nxor %r14, %r14\nxor %r13, %r13\nxor %r12, %r12\nxor %r11, %r11\nxor %r10, %r10\nxor %r9, %r9\nxor %r8, %r8\nxor %ebp, %ebp\nxor %edx, %edx\nxor %ecx, %ecx\nxor %ebx, %ebx\nxor %eax, %eax\n" );

		// run the task in exception mode
		__asm__ volatile( "iretq" );
	}
}

struct KERNEL_STRUCTURE_TASK *kernel_task_active() {
	// from list of active tasks on individual logical processors
	// select currently processed position relative to current logical processor
	return (struct KERNEL_STRUCTURE_TASK *) kernel -> task_cpu_address[ kernel_lapic_id() ];
}

struct KERNEL_STRUCTURE_TASK *kernel_task_add( uint8_t *name, uint8_t length ) {
	// deny access to modification of job queue
	while( __sync_val_compare_and_swap( &kernel -> task_semaphore, UNLOCK, LOCK ) );

	// find an free entry
	for( uint64_t t = 0; t < KERNEL_TASK_limit; t++ ) {
		// free entry?
		if( kernel -> task_base_address[ t ].flags ) continue;	// no

		// mark entry as "in use""
		kernel -> task_base_address[ t ].flags = KERNEL_TASK_FLAG_secured;

		// ID of new job
		kernel -> task_base_address[ t ].pid = ++kernel -> task_id;

		// parent ID
		kernel -> task_base_address[ t ].pid_parent = kernel_task_pid();

		// task doesn't use memory, yet
		kernel -> task_base_address[ t ].page = EMPTY;

		// number of characters representing process name
		kernel -> task_base_address[ t ].name_length = length;

		// set process name
		if( length > LIB_SYS_TASK_NAME_length ) length = LIB_SYS_TASK_NAME_length;
		for( uint16_t n = 0; n < length; n++ ) kernel -> task_base_address[ t ].name[ n ] = name[ n ]; kernel -> task_base_address[ t ].name[ length ] = EMPTY;

		// number of jobs in queue
		kernel -> task_count++;

		// free access to job queue
		kernel -> task_semaphore = UNLOCK;

		// new task initiated
		return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ t ];
	}

	// free access to job queue
	kernel -> task_semaphore = UNLOCK;

	// no free entry
	return EMPTY;
}

uintptr_t kernel_task_by_id( uint64_t pid ) {
	// find the task
	for( uint64_t t = 0; t < KERNEL_TASK_limit; t++ )
		// task found?
		if( kernel -> task_base_address[ t ].pid == pid ) return (uintptr_t) &kernel -> task_base_address[ t ];

	// task not found
	return EMPTY;
}

uint64_t kernel_task_pid() {
	// based on ID of active logical processor
	// get from list of active jobs, number of current record in job queue
	struct KERNEL_STRUCTURE_TASK *task = (struct KERNEL_STRUCTURE_TASK *) kernel -> task_cpu_address[ kernel_lapic_id() ];

	// get ID of process
	return task -> pid;
}

struct KERNEL_STRUCTURE_TASK *kernel_task_select( uint64_t entry ) {
	// block possibility of modifying the tasks, only 1 CPU at a time
	while( __sync_val_compare_and_swap( &kernel -> task_cpu_semaphore, UNLOCK, LOCK ) );

	// search until found
	while( TRUE ) {
		// search in task queue for a ready-to-do task
		for( ; entry < KERNEL_TASK_limit; entry++ ) {
			// task available for processing?
			if( kernel -> task_base_address[ entry ].flags & KERNEL_TASK_FLAG_active && ! (kernel -> task_base_address[ entry ].flags & KERNEL_TASK_FLAG_exec) ) {	// yes
				// a dormant task?
				if( kernel -> task_base_address[ entry ].sleep > kernel -> time_rtc ) continue;

				// mark the task as performed by current logical processor
				kernel -> task_base_address[ entry ].flags |= KERNEL_TASK_FLAG_exec;

				// ID of logical processor processing this task
				// kernel -> task_base_address[ entry ].cpu = (uint64_t) kernel_lapic_id();

				// unlock access to modification of the tasks
				kernel -> task_cpu_semaphore = UNLOCK;

				// return address of selected task from the queue
				return (struct KERNEL_STRUCTURE_TASK *) &kernel -> task_base_address[ entry ];
			}
		}

		// start from the begining
		entry = 0;
	}
}
