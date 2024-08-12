/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_cmd() {
	// kernel file properties
	struct limine_file *limine_file = limine_kernel_file_request.response -> kernel_file;

	// retrieve file name to execute later
	for( uint16_t i = 0; i < lib_string_length( (uint8_t *) limine_file -> cmdline ); i++ )
		kernel_init_exec_file[ i ] = limine_file -> cmdline[ i ];
}
