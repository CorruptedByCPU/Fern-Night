/*=====================WM_=========================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void wm_zone() {
	// no zones no list?
	if( ! wm_zone_count ) return;

	// process all zones on list
	for( uint64_t z = 0; z < wm_zone_count; z++ ) {
		// the zone already has an assigned object?
		if( wm_zone_base_address[ z ].object_address ) continue;	// yes

		// analyze the zone against each object in the list
		for( uint64_t l = 0; l < wm_list_count; l++ ) {
			// get a pointer to an object array record from the list
			struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ l ];

			// the object is invisible?
			if( ! (object -> flags & WM_OBJECT_FLAG_visible) ) continue;	// yes

			// zone and object overlap?
			if( object -> x + object -> width <= wm_zone_base_address[ z ].x ) continue;	// no
			if( object -> y + object -> height <= wm_zone_base_address[ z ].y ) continue;	// no
			if( object -> x >= wm_zone_base_address[ z ].x + wm_zone_base_address[ z ].width ) continue;	// no
			if( object -> y >= wm_zone_base_address[ z ].y + wm_zone_base_address[ z ].height ) continue;	// no

			// temporary zone
			struct WM_STRUCTURE_ZONE tmp_zone = wm_zone_base_address[ z ];

			// left edge of the zone in front of the left edge of the object?
			if( tmp_zone.x < object -> x ) {
				// correct the zone
				tmp_zone.width -= object -> x - tmp_zone.x;
				tmp_zone.x = object -> x;
			}

			// top edge of the zone in front of the top edge of the object?
			if( tmp_zone.y < object -> y ) {
				// correct the zone
				tmp_zone.height -= object -> y - tmp_zone.y;
				tmp_zone.y = object -> y;
			}

			// right edge of the zone behind the right edge of the object?
			if( (tmp_zone.x + tmp_zone.width) > (object -> x + object -> width) ) {
				// correct the zone
				tmp_zone.width -= (tmp_zone.x + tmp_zone.width) - (object -> x + object -> width);
			}

			// lower edge of the zone behind the lower edge of the object?
			if( (tmp_zone.y + tmp_zone.height) > (object -> y + object -> height) ) {
				// correct the zone
				tmp_zone.height -= (tmp_zone.y + tmp_zone.height) - (object -> y + object -> height);
			}

			// fill the zone with the given object
			tmp_zone.object_address = wm_list_base_address[ l ];
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &tmp_zone, TRUE );
		}
	}
}

void wm_zone_insert( struct WM_STRUCTURE_ZONE *ptr, uint8_t parsed ) {
	// zone is in framebuffer space?
	if( ptr -> x >= (int16_t) wm_object_framebuffer.width ) return;	// no
	if( ptr -> y >= (int16_t) wm_object_framebuffer.height ) return;	// no
	if( ptr -> x + ptr -> width <= 0 ) return;	// no
	if( ptr -> y + ptr -> height <= 0 ) return;	// no

	// there is space on list?
	if( wm_zone_count < WM_ZONE_limit ) {
		// truncate the portions of the zone that go beyond the screen space on the X axis
		if( ptr -> x < 0 ) {
			// if it is on the left side
			wm_zone_base_address[ wm_zone_count ].width = ptr -> width - (~ptr -> x + 1);
			wm_zone_base_address[ wm_zone_count ].x = 0;
		} else if( ptr -> x + ptr -> width >= (int16_t) wm_object_framebuffer.width ) {
			// if it is sticking out to the right
			wm_zone_base_address[ wm_zone_count ].x = ptr -> x;
			wm_zone_base_address[ wm_zone_count ].width = ptr -> width - ((ptr -> x + ptr -> width) - (int16_t) wm_object_framebuffer.width);
		} else {
			// the entire screen space on the X axis
			wm_zone_base_address[ wm_zone_count ].x = ptr -> x;
			wm_zone_base_address[ wm_zone_count ].width = ptr -> width;
		}

		// truncate the portions of the zone that go beyond the screen space on the Y axis
		if( ptr -> y < 0 ) {
			// if it is on the left side
			wm_zone_base_address[ wm_zone_count].height = ptr -> height - (~ptr -> y + 1);
			wm_zone_base_address[ wm_zone_count].y = 0;
		} else if( ptr -> y + ptr -> height >= (int16_t) wm_object_framebuffer.height ) {
			// if it is sticking out to the right
			wm_zone_base_address[ wm_zone_count ].y = ptr -> y;
			wm_zone_base_address[ wm_zone_count ].height = ptr -> height - ((ptr -> y + ptr -> height) - (int16_t) wm_object_framebuffer.height);
		} else {
			// the entire screen space on the Y axis
			wm_zone_base_address[ wm_zone_count ].y = ptr -> y;
			wm_zone_base_address[ wm_zone_count ].height = ptr -> height;
		}

		// object assigned to the zone?
		if( parsed ) wm_zone_base_address[ wm_zone_count ].object_address = ptr -> object_address;
		else
			// zone to be processed
			wm_zone_base_address[ wm_zone_count ].object_address = EMPTY;

		// the number of zones in the list has increased
		wm_zone_count++;
	}
}