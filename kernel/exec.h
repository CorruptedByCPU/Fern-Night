/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_EXEC
	#define KERNEL_EXEC

	#define	KERNEL_EXEC_FAIL_task		1
	#define	KERNEL_EXEC_FAIL_cr3		2

	#define	KERNEL_EXEC_BASE_address	0x0000000000100000

	#define	KERNEL_EXEC_STACK_SIZE_page	1

	#define	KERNEL_EXEC_STACK_pointer	0x0000700000000000
	#define	KERNEL_EXEC_STACK_address	KERNEL_EXEC_STACK_pointer - (KERNEL_EXEC_STACK_SIZE_page << STATIC_PAGE_SIZE_shift)

	struct	KERNEL_EXEC_STRUCTURE {
		uint64_t	pid;
		uintptr_t	task_or_status;
	};

	uint64_t kernel_exec( uint8_t *path, uint8_t length, uint8_t stream );
	void kernel_exec_link( struct LIB_ELF_STRUCTURE *elf, uintptr_t executable_space );
#endif
