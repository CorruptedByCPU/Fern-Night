/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

__attribute__(( preserve_most ))
void kernel_hpet_uptime( void ) {
	// increase the real-time controller invocation count
	kernel -> hpet_microtime++;

	// accept the current interrupt call
	kernel_lapic_accept();
}