/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int console_init( uint64_t argc, uint8_t *argv[] ) {
	// get our PID number
	console_pid = lib_sys_task_pid();

	// request properties of Framebuffer
	lib_sys_framebuffer( (struct LIB_SYS_STRUCTURE_FRAMEBUFFER *) &console_framebuffer );

	// are we the master of puppets?
	if( console_pid == console_framebuffer.pid ) {
		// look at me, i'm the captain now
		console_the_master_of_puppets = TRUE;

		// prepate space for cache
		console_cache = (uint32_t *) lib_sys_memory_alloc( console_framebuffer.scanline_byte * console_framebuffer.height_pixel );

		// update terminal properties with new cache
		console_terminal.base_address = console_cache;
		console_terminal.width = console_framebuffer.width_pixel;
		console_terminal.height = console_framebuffer.height_pixel;
		console_terminal.scanline_pixel = console_framebuffer.scanline_byte >> STATIC_VIDEO_DEPTH_shift;

		// initialize terminal library
		lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

		// update metadata properties of stream
		console_stream_meta.width = console_terminal.width_char;
		console_stream_meta.height = console_terminal.height_char;
	} else {
		// initialize interface of application
		lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &console_interface );

		// find element with ID:1 and point event function to close();
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &console_interface, 1 );
		if( element ) element -> event = (void *) close;

		// initialize terminal library
		console_terminal.base_address = console_interface.base_address + (LIB_INTERFACE_HEADER_HEIGHT_pixel * CONSOLE_WINDOW_width_pixel) + 1;

		// initialize terminal library
		lib_terminal( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );
	}

	// update meta data of input stream
	lib_sys_stream_set( (uint8_t *) &console_stream_meta, LIB_SYS_STREAM_in );

	// assign memory for received stream content
	console_stream_in = (uint8_t *) lib_sys_memory_alloc( LIB_SYS_STREAM_SIZE_byte );

	// console initialized
	return 0;
}