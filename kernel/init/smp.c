/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_smp() {
	// Other CPU are available?
	if( limine_smp_request.response == NULL ) return;	// no

	// amount of running APs
	uint64_t smp_count = EMPTY;

	// initialize every Local CPU
	for( uint64_t i = EMPTY; i < limine_smp_request.response -> cpu_count; i++ ) {
		// do not initialize BSP processor, yet
		if( limine_smp_request.response -> cpus[ i ] -> lapic_id == kernel_lapic_id() ) continue;

		// set first function to execute by Local CPU
		limine_smp_request.response -> cpus[ i ] -> goto_address = (void *) &kernel_init_ap;

		// AP is running
		smp_count++;

		// wait for AP initialization
		while( kernel -> cpu_count < smp_count );

		// remember CPU ID if greater than previous one
		if( kernel -> lapic_last_id < limine_smp_request.response -> cpus[ i ] -> lapic_id ) kernel -> lapic_last_id = limine_smp_request.response -> cpus[ i ] -> lapic_id;
	}

	// show information about initialized CPUs
	kernel_log( "+%d AP(s) initialized.\n", kernel -> cpu_count );
}