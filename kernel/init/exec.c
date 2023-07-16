/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_exec() {
	// execute Init
	kernel_exec( kernel_init_exec_file, lib_string_length( kernel_init_exec_file ), LIB_SYS_STREAM_FLOW_out_to_parent_out );
}
