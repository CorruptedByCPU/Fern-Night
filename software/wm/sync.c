/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_sync() {
	// the buffer has been modified?
	if( wm_object_framebuffer.flags & WM_OBJECT_FLAG_flush ) {
		// pointer to the buffer space
		uint32_t *object = (uint32_t *) wm_object_framebuffer.base_address;

		// copy the contents of the buffer to the memory space of the graphics card
		for( uint32_t pixel = 0; pixel < (wm_object_framebuffer.width * wm_object_framebuffer.height); pixel++ )
				wm_framebuffer.base_address[ pixel ] = object[ pixel ];

		// buffer synchronized
		wm_object_framebuffer.flags &= ~WM_OBJECT_FLAG_flush;
	}
}