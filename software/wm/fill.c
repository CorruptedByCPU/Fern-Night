/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_fill() {
	// no zones on the list?
	if( ! wm_zone_count ) return;

	// process each entry in the fill list
	for( uint64_t z = 0; z < wm_zone_count; z++ ) {
		// zone to be processed?
		if( wm_zone_base_address[ z ].object_address ) {
			// get a pointer to the object
			struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_zone_base_address[ z ].object_address;

			// fill the corrected fragment with the given object
			uint32_t *source = (uint32_t *) object -> base_address;
			uint32_t *target = (uint32_t *) wm_object_framebuffer.base_address;
			for( uint64_t y = wm_zone_base_address[ z ].y; y < wm_zone_base_address[ z ].height + wm_zone_base_address[ z ].y; y++ ) {
				for( uint64_t x = wm_zone_base_address[ z ].x; x < wm_zone_base_address[ z ].width + wm_zone_base_address[ z ].x; x++ ) {
					// color properties
					uint32_t color_current = target[ (y * wm_object_framebuffer.width) + x ];
					uint32_t color_new = source[ (x - object -> x) + (object -> width * (y - object -> y)) ];

					// perform the operation based on the alpha channel
					switch( color_new >> (STATIC_MOVE_HIGH_TO_AX_shift + STATIC_MOVE_HIGH_TO_AL_shift) ) {
						// no alpha channel
						case (uint8_t) STATIC_MAX_unsigned: {
							target[ (y * wm_object_framebuffer.width) + x ] = color_new; continue;
						}

						// transparent color
						case EMPTY: continue;
					}

					// calculate the color based on the alpha channel
					target[ (y * wm_object_framebuffer.width) + x ] = lib_color_blend( color_current, color_new );
					// if( object -> pid == wm_pid_number ) target[ (y * wm_object_framebuffer.width) + x ] = lib_color_blend( color_current, color_new );
					// else target[ (y * wm_object_framebuffer.width) + x ] = color_new;
				}
			}

			// remove fill from list
			wm_zone_base_address[ z ].x = EMPTY;
			wm_zone_base_address[ z ].y = EMPTY;
			wm_zone_base_address[ z ].width = EMPTY;
			wm_zone_base_address[ z ].height = EMPTY;
			wm_zone_base_address[ z ].object_address = EMPTY;

			// synchronize framebuffer object
			wm_object_framebuffer.flags |= WM_OBJECT_FLAG_flush;
		}
	}

	// all fragments in the list have been processed
	wm_zone_count = EMPTY;
}
