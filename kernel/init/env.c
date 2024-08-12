/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_env() {
	// all variables/functions are available only for kernel, drivers and daemons

	// Log management functions
	kernel -> log = (void *) kernel_log;

	// variables of Video management functions
	kernel -> framebuffer_base_address = (uint32_t *) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;
	kernel -> framebuffer_width_pixel = (uint16_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
	kernel -> framebuffer_height_pixel = (uint16_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
	kernel -> framebuffer_pitch_byte = (uint32_t) limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;

	// show information about video
	kernel -> log( "Framebuffer at 0x%x (%dx%d)\n", kernel -> framebuffer_base_address, kernel -> framebuffer_width_pixel, kernel -> framebuffer_height_pixel );

	// share functions about memory management
	kernel -> memory_release = (void *) kernel_memory_release;

	// paging management
	kernel -> page_deconstruction = (void *) kernel_page_deconstruction;

	// stream management
	kernel -> stream_release = (void *) kernel_stream_release;
}
