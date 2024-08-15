/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_limit		(8192 / sizeof( struct KERNEL_STRUCTURE_TASK ))	// hard limit

	#define	KERNEL_TASK_EFLAGS_cf		0b000000000000000000000001
	#define	KERNEL_TASK_EFLAGS_df		0b000000000000010000000000
	#define	KERNEL_TASK_EFLAGS_if		0b000000000000001000000000
	#define	KERNEL_TASK_EFLAGS_zf		0b000000000000000001000000
	#define	KERNEL_TASK_EFLAGS_default	KERNEL_TASK_EFLAGS_if

	#define	KERNEL_TASK_FLAG_active		0b0000000000000001
	#define	KERNEL_TASK_FLAG_exec		0b0000000000000010
	#define	KERNEL_TASK_FLAG_close		0b0000000000000100
	#define	KERNEL_TASK_FLAG_module		0b0000000000001000
	#define	KERNEL_TASK_FLAG_thread		0b0000000000010000
	#define	KERNEL_TASK_FLAG_sleep		0b0000000000100000
	#define	KERNEL_TASK_FLAG_init		0b0100000000000000
	#define	KERNEL_TASK_FLAG_secured	0b1000000000000000

	#define	KERNEL_TASK_NAME_limit		254

	#define	KERNEL_TASK_STACK_pointer	KERNEL_LIBRARY_base_address
	#define	KERNEL_TASK_STACK_limit		STD_PAGE_byte

	struct	KERNEL_STRUCTURE_TASK {
		uintptr_t		cr3;
		uintptr_t		rsp;
		int64_t			pid;
		int64_t			pid_parent;
		uint64_t		sleep;
		uint64_t		storage;
		uint64_t		directory;
		uint64_t		page;	// amount of pages assigned to process
		uint8_t			page_type;
		uint64_t		stack;	// size of stack in Pages
		uint64_t		time;
		uint64_t		time_previous;
		struct KERNEL_STRUCTURE_STREAM *stream_in;
		struct KERNEL_STRUCTURE_STREAM *stream_out;
		uint32_t		*memory_map;
		volatile uint16_t	flags;
		uint8_t			name_length;
		uint8_t			name[ KERNEL_TASK_NAME_limit + 1 ];
	} __attribute__( (packed) );

	// main task switch function
	void kernel_task( void );
		// support function for kernel_task( uint64_t table_entry_ID );
		struct KERNEL_STRUCTURE_TASK *kernel_task_select( uint64_t entry );

	// returns process ID of running task
	uint64_t kernel_task_pid();

	// returns new task descriptor pointer
	struct KERNEL_STRUCTURE_TASK *kernel_task_add( uint8_t *name, uint8_t length );

	// returns current task descriptor pointer
	struct KERNEL_STRUCTURE_TASK *kernel_task_active();

	// returns task descriptor pointer by process ID
	uintptr_t kernel_task_by_id( uint64_t pid );
#endif