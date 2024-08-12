/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_FREE
	#define	KERNEL_INIT_FREE

	struct	KERNEL_INIT_STRUCTURE_FREE {
		uint64_t	size_page;
		uintptr_t	base_address;
	};

	void kernel_init_free();
#endif