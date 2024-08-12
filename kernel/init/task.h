/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_TASK
	#define	KERNEL_INIT_TASK

	// external routine (assembly language)
	extern void kernel_task_entry( void );

	void kernel_init_task();
#endif