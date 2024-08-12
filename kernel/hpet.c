/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( preserve_most ))
void kernel_hpet_uptime( void ) {
	// increase the real-time controller invocation count
	kernel -> time_rtc++;

	// accept the current interrupt call
	kernel_lapic_accept();
}