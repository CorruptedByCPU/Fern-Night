/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_PAGE
	#define	KERNEL_INIT_PAGE
	
	void kernel_init_page();
	void kernel_init_page_high_half( uint64_t address, uint64_t pages, uint16_t flags );
#endif