/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_TASK
	#define	KERNEL_TASK

	#define	KERNEL_TASK_limit		32
	#define KERNEL_TASK_TABLE_LENGTH_page	8

	#define	KERNEL_TASK_FLAG_active		0b0000000000000001
	#define	KERNEL_TASK_FLAG_closed		0b0000000000000010
	#define	KERNEL_TASK_FLAG_daemon		0b0000000000000100
	#define	KERNEL_TASK_FLAG_exec		0b0000000000001000
	#define	KERNEL_TASK_FLAG_secured	0b0000000000010000
	#define	KERNEL_TASK_FLAG_thread		0b0000000000100000
	#define	KERNEL_TASK_FLAG_sleep		0b0000000100000000
	#define	KERNEL_TASK_FLAG_init		0b0000010000000000

	#define	KERNEL_TASK_EFLAGS_cf		0b000000000000000000000001
	#define	KERNEL_TASK_EFLAGS_zf		0b000000000000000001000000
	#define	KERNEL_TASK_EFLAGS_if		0b000000000000001000000000
	#define	KERNEL_TASK_EFLAGS_df		0b000000000000010000000000
	#define	KERNEL_TASK_EFLAGS_default	KERNEL_TASK_EFLAGS_if

	struct	KERNEL_TASK_STRUCTURE {
		uintptr_t			cr3;
		uintptr_t			rsp;
		uint64_t			pid;
		uint64_t			pid_parent;
		uint64_t			sleep;
		uint32_t			*memory_map;
		uint64_t			page;
		struct KERNEL_STREAM_STRUCTURE	*stream_in;
		struct KERNEL_STREAM_STRUCTURE	*stream_out;
		uint64_t			directory;
		volatile uint16_t		flags;
		uint8_t				storage;
		uint8_t				cpu;
		uint64_t			stack;
		uint8_t				length;
		uint8_t				name[ LIB_SYS_TASK_NAME_length + 1 ];
	} __attribute__( (packed) );

	// main task switch function
	void kernel_task( void );
		// support function for kernel_task( uint64_t table_entry_ID );
		struct KERNEL_TASK_STRUCTURE *kernel_task_select( uint64_t entry );

	// returns process ID of running task
	uint64_t kernel_task_pid();

	// returns new task descriptor pointer
	struct KERNEL_TASK_STRUCTURE *kernel_task_add( uint8_t *name, uint8_t length );

	// returns current task descriptor pointer
	struct KERNEL_TASK_STRUCTURE *kernel_task_active();

	// returns task descriptor pointer by process ID
	uintptr_t kernel_task_by_id( uint64_t pid );
#endif