/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_object() {
	// search array after object to redraw
	for( uint16_t i = 0; i < wm_list_count; i++ ) {
		// get the properties of the object
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ];
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) ((uintptr_t) object -> base_address - sizeof( struct WM_STRUCTURE_DESCRIPTOR ));

		// hide object if minimize request is set
		if( descriptor -> flags & WM_DESCRIPTOR_FLAG_minimize ) {
			// recycle the visible space of the object
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) object, FALSE );

			// hide object
			object -> flags &= ~WM_OBJECT_FLAG_visible;
			object -> flags ^= WM_OBJECT_FLAG_hide;

			// remove minimize flag
			descriptor -> flags &= ~WM_DESCRIPTOR_FLAG_minimize;

			// and select new active object
			wm_object_activate();

			// one of objects was modified
			wm_taskbar_semaphore = TRUE;
		} else

		// hide the object or the object is visible and has updated its content?
		if( object -> flags & WM_OBJECT_FLAG_hide || ((object -> flags & WM_OBJECT_FLAG_visible) && ((object -> flags & WM_OBJECT_FLAG_flush) || (descriptor -> flags & WM_DESCRIPTOR_FLAG_flush))) ) {
			// recycle the visible space of the object
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) object, FALSE );

			// the object was submitted for processing
			object -> flags ^= WM_OBJECT_FLAG_hide;
			object -> flags &= ~WM_OBJECT_FLAG_flush;
			descriptor -> flags &= ~WM_DESCRIPTOR_FLAG_flush;
		} else {
			// if the object needs to be removed but is visible
			if( object -> flags & WM_OBJECT_FLAG_destroy && object -> flags & WM_OBJECT_FLAG_visible ) {
				// hide the object
				object -> flags &= ~WM_OBJECT_FLAG_visible;

				// recycle the space under the object
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) object, FALSE );

				// select the new active object
				wm_object_activate();
			} else
				// if the object is no longer visible
				if( object -> flags & WM_OBJECT_FLAG_destroy )
					// delete the object
					wm_object_remove( object );
		}

		// process owning object is still active?
		if( ! lib_sys_task_status( object -> pid ) ) {
			// object marked?
			if( object -> flags & WM_OBJECT_FLAG_destroy ) {
				// release object
				wm_object_remove( object );

				// select the new active object
				wm_object_activate();

				// one of objects was modified
				wm_taskbar_semaphore = TRUE;
			} else {
				// hide object
				object -> flags |= WM_OBJECT_FLAG_hide;
				object -> flags &= ~WM_OBJECT_FLAG_visible;

				// mark for delete
				object -> flags |= WM_OBJECT_FLAG_destroy;
			}
		}
	}
}

void wm_object_activate() {
	// parse object list as far as to arbiter
	for( uint16_t i = 0; i < wm_list_count; i++ ) {
		// object properties
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ];

		// arbiter object?
		if( object -> flags & WM_OBJECT_FLAG_arbiter ) break;	// yes

		// object is visible?
		if( object -> flags & WM_OBJECT_FLAG_visible ) wm_object_active = object;
	}
}

uintptr_t wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t flags ) {
	// search for empty record in table
	for( uint64_t i = 0; i < WM_OBJECT_limit; i++ ) {
		// free of use?
		if( ! wm_object_base_address[ i ].flags ) {
			// set flags for window
			wm_object_base_address[ i ].flags = flags | WM_OBJECT_FLAG_exist;

			// fill record with window properties
			wm_object_base_address[ i ].x = x;
			wm_object_base_address[ i ].y = y;
			wm_object_base_address[ i ].width = width;
			wm_object_base_address[ i ].height = height;
			wm_object_base_address[ i ].size_byte = (width * height) << STATIC_VIDEO_DEPTH_shift;
			wm_object_base_address[ i ].id = ++wm_object_id;
			wm_object_base_address[ i ].pid = EMPTY;

			// set default object name
			wm_object_base_address[ i ].length = 0;
			for( uint8_t n = 0; n < sizeof( wm_object_name_default ) - 1; n++ ) wm_object_base_address[ i ].name[ n ] = wm_object_name_default[ n ];

			// register window on object list
			wm_object_insert( (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ] );

			// newly created object becomes active
			wm_object_active = (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ];

			// ready
			return (uint64_t) &wm_object_base_address[ i ];
		}
	}

	// error
	return EMPTY;
}

struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden ) {
	// find the object at the current cursor coordinates
	uint64_t *list = (uint64_t *) wm_list_base_address;
	for( uint16_t i = wm_list_count - 1; i >= 0; i-- ) {
		// get the properties of the object
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) list[ i ];

		// the object is visible? (or include hidden ones too)
		if( hidden || object -> flags & WM_OBJECT_FLAG_visible ) {
			// the cursor is in the object's space on the X axis?
			if( object -> x > x ) continue;	// no
			if( object -> y > y ) continue;	// no
			if( (object -> x + object -> width) <= x ) continue;	// no
			if( (object -> y + object -> height) <= y ) continue;	// no

			// the object is the cursor?
			if( object -> flags & WM_OBJECT_FLAG_cursor ) continue;	// yes, ignore it

			// return a pointer to an object in an array
			return object;
		}
	}

	// dependent object not found
	return EMPTY;
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// find an empty space in object array
	for( uint16_t i = 0; i < WM_LIST_limit; i++ ) {
		// position on list is taken?
		if( wm_list_base_address[ i ] ) {
			// taken by an arbitrator?
			struct WM_STRUCTURE_OBJECT *current = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ];
			if( (current -> flags & WM_OBJECT_FLAG_arbiter || current -> flags & WM_OBJECT_FLAG_cursor) && ! (object -> flags & WM_OBJECT_FLAG_cursor) ) {
				// move all next objects on list (including the arbitrator) one position forward
				for( uint64_t j = WM_LIST_limit; j > i; j-- )
					wm_list_base_address[ j ] = wm_list_base_address[ j - 1 ];

				// put the object into the list
				wm_list_base_address[ i ] = (uintptr_t) object;

				// increase number of objects on list
				wm_list_count++;

				// operation performed
				return;
			}
		} else {
			// put the object onto the list
			wm_list_base_address[ i ] = (uintptr_t) object;

			// increase number of objects on list
			wm_list_count++;

			// operation performed
			return;
		}
	}
}

void wm_object_move( struct WM_STRUCTURE_OBJECT *object, int16_t x, int16_t y ) {
	// the object can be moved along the X, Y axis?
	if( ! (object -> flags & WM_OBJECT_FLAG_fixed_xy) ) {
		// record the zones exposed by the displacement of the object
		struct WM_STRUCTURE_ZONE zone;
		zone.x = object -> x;
		zone.y = object -> y;
		zone.width = object -> width;
		zone.height = object -> height;

		// a movement on the X axis has occurred?
		if( x != 0 ) {
			// the new position of the object
			object -> x += x;

			// the X axis offset is positive?
			if( x > 0 ) {	// yes
				// width of the exposed zone
				zone.width = x;
			} else if( x < 0 ) {	// negative
				// position and width of the exposed zone
				zone.x = object -> x + object -> width;
				zone.width = ~x + 1;
			}

			// register the zone in the list
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );

			// correct the position of the next zone
			zone.x = object -> x;
			zone.width = object -> width;
		}

		// displacement occurred on the Y axis?
		if( y != 0 ) {
			// the new position of the object
			object -> y += y;

			// the Y axis offset is positive?
			if( y > 0 ) {	// yes
				// height of the exposed fragment
				zone.height = y;
			} else if( y < 0 ) {	// negative
				// position and height of the exposed fragment
				zone.y = object -> y + object -> height;
				zone.height = ~y + 1;
			}

			// register the zone in the list
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, FALSE );
		}

		// the object has been moved
		object -> flags |= WM_OBJECT_FLAG_flush;
	}
}

void wm_object_move_up( struct WM_STRUCTURE_OBJECT *selected ) {
	// find the object in the list
	for( uint16_t i = 0; i < wm_list_count; i++ ) {
		// the object in question?
		if( wm_list_base_address[ i ] == (uintptr_t) selected ) {
			// move all objects in the list back one position
			for( uint16_t o = i + 1; o < wm_list_count; o++ ) {
				// the next object is an arbiter?
				struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ o ];
				if( object -> flags & WM_OBJECT_FLAG_arbiter || object -> flags & WM_OBJECT_FLAG_cursor ) break;

				// no, move the next object to the current position
				wm_list_base_address[ i++ ] = wm_list_base_address[ o ];
			}

			// put the object back in its new position
			wm_list_base_address[ i ] = (uintptr_t) selected;

			// displacement performed
			return;
		}
	}
}

void wm_object_remove( struct WM_STRUCTURE_OBJECT *object ) {
	// remove the object from the list of objects
	for( uint16_t i = 0; i < wm_list_count; i++ )
		// the object in question?
		if( wm_list_base_address[ i ] == (uintptr_t) object )
			// move all objects in the list back one position to the one being deleted
			for( uint16_t o = i + 1; o <= wm_list_count; o++ )
				wm_list_base_address[ i++ ] = wm_list_base_address[ o ];

	// free the object's data space
	lib_sys_memory_release( (uintptr_t) object -> base_address, object -> size_byte );

	// release the entry in the object table
	object -> flags = EMPTY;

	// number of objects decreased
	wm_list_count--;

	// array of objects modified
	wm_object_semaphore = TRUE;
}

struct WM_STRUCTURE_OBJECT *wm_object_by_id( uint8_t id ) {
	// find the window with the given id
	for( uint8_t i = 0; i < wm_list_count; i++ ) {
		// window id located?
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ];
		if( object -> id == id ) return object;	// yes
	}

	// no window with the given ID was found
	return EMPTY;
}

void wm_object_fragile() {
	// find all fragile objects
	uint64_t *list = (uint64_t *) wm_list_base_address;
	for( uint16_t i = 0; i < wm_list_count; i++ ) {
		// get the properties of the object
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) list[ i ];

		// the object is fragile?
		if( object -> flags & WM_OBJECT_FLAG_fragile ) {
			// hide object
			object -> flags &= ~WM_OBJECT_FLAG_visible;

			// redraw object position
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) object, FALSE );
		}
	}
}