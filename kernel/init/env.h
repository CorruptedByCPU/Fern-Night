/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_ENV
	#define	KERNEL_INIT_ENV

	// external pointer (assembly language)
	extern void kernel_service_list;

	void kernel_init_env();
#endif