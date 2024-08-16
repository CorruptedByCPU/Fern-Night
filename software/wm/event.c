/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event() {
	// there are messages for us?
	if( (lib_sys_ipc_receive( (struct STD_IPC_STRUCTURE *) &wm_message, LIB_SYS_IPC_TYPE_WINDOW_MANAGER )) ) {
		// check message type
		struct WM_STRUCTURE_IPC *ipc = (struct WM_STRUCTURE_IPC *) &wm_message.data;

		// create new window?
		if( ipc -> function == WM_IPC_FUNCTION_create ) {
			// properties of new window
			struct WM_STRUCTURE_IPC_WINDOW_create *create = (struct WM_STRUCTURE_IPC_WINDOW_create *) &wm_message.data;

			// find parent by ID
			struct WM_STRUCTURE_OBJECT *parent = wm_object_by_id( create -> id );

			// modify object position
			if( parent ) {
				// set relative position of new object
				create -> x += parent -> x;
				create -> y += parent -> y;
			} else {
				// default object position
				create -> x = (wm_object_workbench -> width / 2) - (create -> width / 2);
				create -> y = (wm_object_workbench -> height / 2) - (create -> height / 2);
			}

			// create an object with given parameters
			struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_object_create( create -> x, create -> y, create -> width, create -> height, WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush );

			// set object owner
			object -> pid = wm_message.source;

			// extend size of object by descriptor
			object -> size_byte += sizeof( struct WM_STRUCTURE_DESCRIPTOR );
			object -> base_address = (uint32_t *) lib_sys_memory_alloc( object -> size_byte ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

			// prepare response
			struct WM_STRUCTURE_IPC_WINDOW_properties properties = { EMPTY };
			properties.ipc.type = LIB_SYS_IPC_TYPE_WINDOW_MANAGER;
			properties.ipc.function = WM_IPC_FUNCTION_properties;

			// return ID of window
			properties.id = object -> id;

			// and descriptor
			properties.descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) lib_sys_memory_share( MACRO_PAGE_ALIGN_DOWN( (uint64_t) object -> base_address ), object -> size_byte, wm_message.source );

			// send answer back to process
			lib_sys_ipc_send( wm_message.source, (uint8_t *) &properties );

			// one of the objects was modified
			wm_taskbar_semaphore = TRUE;
		}

		// rename window?
		if( ipc -> function == WM_IPC_FUNCTION_rename ) {
			// properties of new window
			struct WM_STRUCTURE_IPC_WINDOW_rename *rename = (struct WM_STRUCTURE_IPC_WINDOW_rename *) &wm_message.data;

			// search for requested window
			struct WM_STRUCTURE_OBJECT *object = wm_object_by_id( rename -> id );

			// if found
			if( object ) {
				// rename object
				object -> length = rename -> length; if( object -> length > WM_OBJECT_NAME_length ) object -> length = WM_OBJECT_NAME_length;
				for( uint8_t n = 0; n < object -> length; n++ ) object -> name[ n ] = rename -> name[ n ];

				// one of the objects was modified
				wm_taskbar_semaphore = TRUE;
			}
		}

		// close window?
		if( ipc -> function == WM_IPC_FUNCTION_close ) {
			// properties of window
			struct WM_STRUCTURE_IPC_WINDOW_close *close = (struct WM_STRUCTURE_IPC_WINDOW_close *) &wm_message.data;

			// find object by ID
			struct WM_STRUCTURE_OBJECT *object = wm_object_by_id( close -> id );

			// it's our window?
			if( wm_message.source == object -> pid ) {
				// redraw space under of object
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) object, FALSE );

				// release object
				wm_object_remove( object );

				// select the new active object
				wm_object_activate();

				// one of objects was modified
				wm_taskbar_semaphore = TRUE;
			}
		}
	}

	//--------------------------------------------------------------------------
	// get a key from the keyboard buffer
	uint16_t key = lib_sys_keyboard();

	// key status change?
	if( key ) {
		// take action
		switch( key ) {
			case STATIC_ASCII_BACKSLASH:
				// start new console
				lib_sys_exec( (uint8_t *) &wm_exec_console, sizeof( wm_exec_console ) - 1, LIB_SYS_STREAM_FLOW_out_to_parent_out );

				// ready
				break;
			case DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT:
				// update ALT key status
				wm_keyboard_status_alt_left = DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT;

				// ready
				break;
			case DRIVER_PS2_KEYBOARD_RELEASE_ALT_LEFT:
				// update ALT key status
				wm_keyboard_status_alt_left = DRIVER_PS2_KEYBOARD_RELEASE_ALT_LEFT;

				// ready
				break;
			case DRIVER_PS2_KEYBOARD_PRESS_TAB:
				// if ALT key is on hold
				if( wm_keyboard_status_alt_left == DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT ) {
					// search array after object to activate
					for( uint16_t i = 0; i < wm_list_count; i++ ) {
						// properties of next object
						struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ];

						// only if it is not our object or doesn't have any oth those flags
						if( object -> pid == wm_pid_number || object -> flags & (WM_OBJECT_FLAG_arbiter | WM_OBJECT_FLAG_menu) ) continue;	// ignore it

						// move the object to the top position in the list
						wm_object_move_up( object );

						// show
						object -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;

						// set as active
						wm_object_activate();

						// one of the objects was modified
						wm_taskbar_semaphore = TRUE;

						// done
						break;
					}
				}

				// ready
				break;
		}

		// if the key was different than backslash
		if( key != STATIC_ASCII_BACKSLASH )
			// send to active object
			wm_ipc_keyboard( key );
	}

	//--------------------------------------------------------------------------

	// get properties of mouse
	lib_sys_mouse( (uintptr_t) &wm_mouse );

	// calculate the delta for the X and Y axes
	int16_t delta_x = wm_mouse.x - wm_object_cursor -> x;
	int16_t delta_y = wm_mouse.y - wm_object_cursor -> y;

	//--------------------------------------------------------------------------

	// update cursor position inside objects
	uint64_t *list = (uint64_t *) wm_list_base_address;
	for( uint16_t i = 0; i < wm_list_count; i++ ) {
		// object properties
		struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) list[ i ];

		// objects descriptor properties
		struct WM_STRUCTURE_DESCRIPTOR *descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) &object -> base_address[ -(sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift) ];

		// object is visible?
		if( object -> flags & WM_OBJECT_FLAG_visible ) {
			// X axis
			if( wm_object_cursor -> x + delta_y >= object -> x && wm_object_cursor -> x + delta_x < object -> x + object -> width ) descriptor -> pointer_x = (wm_object_cursor -> x + delta_x) - object -> x;
			else descriptor -> pointer_x = -1;

			// Y asix
			if( wm_object_cursor -> y + delta_y >= object -> y && wm_object_cursor -> y + delta_y < object -> y + object -> height ) descriptor -> pointer_y = (wm_object_cursor -> y + delta_y) - object -> y;
			else descriptor -> pointer_y = -1;
		}
	}

	//--------------------------------------------------------------------------
	// left mouse button pressed?
	if( wm_mouse.status & LIB_SYS_device_mouse_status_left_mouse_button ) {
		// left mouse button held down?
		if( ! wm_mouse_button_left_semaphore ) {	// no
			// save key state
			wm_mouse_button_left_semaphore = TRUE;

			// get a pointer to the object under the cursor
			wm_object_selected = wm_object_find( wm_mouse.x, wm_mouse.y, FALSE );

			// one of the objects was modified
			wm_taskbar_semaphore = TRUE;

			// the object can be moved along the Z axis and left ALT key is not holded?
			if( ! (wm_object_selected -> flags & WM_OBJECT_FLAG_fixed_z) && (wm_keyboard_status_alt_left != DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT) ) {
				// move the object to the top position in the list
				wm_object_move_up( wm_object_selected );

				// redraw the object
				wm_object_selected -> flags |= WM_OBJECT_FLAG_flush;

				// the cursor pointer may be obscured, redraw again
				wm_object_cursor -> flags |= WM_OBJECT_FLAG_flush;
			}

			// make object as active if left ALT key is not holded, but ignore taskbar object
			if( wm_keyboard_status_alt_left != DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT && wm_object_selected != wm_object_taskbar ) wm_object_active = wm_object_selected;

			// cursor at object header position?
			if( wm_object_cursor -> y - (wm_object_selected -> y + delta_y) < LIB_SYS_INTERFACE_HEADER_height_in_pixel && wm_object_cursor -> x - (wm_object_selected -> x + delta_x) < wm_object_selected -> width - (LIB_SYS_INTERFACE_HEADER_height_in_pixel * 3) )
				// yes
				wm_object_selected_semaphore = TRUE;

			// send mouse status to object process
			wm_ipc_mouse( LIB_SYS_device_mouse_status_left_mouse_button );

			// hide all objects with fragile flag
			if( wm_object_selected != wm_object_taskbar ) wm_object_fragile();
		}
	} else {
		// release semaphore
		wm_mouse_button_left_semaphore = FALSE;

		// cursor no longer at object header
		wm_object_selected_semaphore = FALSE;
	}

	//--------------------------------------------------------------------------
	// if cursor pointer movement occurs
	if( delta_x || delta_y ) {
		// display the content under the cursor object
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

		// update cursor position
		wm_object_cursor -> x += delta_x;
		wm_object_cursor -> y += delta_y;

		// redisplay the cursor at the new location
		wm_object_cursor -> flags |= WM_OBJECT_FLAG_flush;

		// if the left ALT key is held down together with the left mouse button
		struct WM_STRUCTURE_OBJECT *window = (struct WM_STRUCTURE_OBJECT *) wm_object_selected;
		if( wm_object_selected && wm_mouse_button_left_semaphore && (wm_object_selected_semaphore || (wm_keyboard_status_alt_left == DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT)) ) {
			// object after movement will be above taskbar?
			if( wm_object_selected -> y + delta_y < (wm_object_taskbar -> y - WM_OBJECT_TASKBAR_HEIGHT_pixel) )
				// move the object along with the cursor pointer
				wm_object_move( wm_object_selected, delta_x, delta_y );
		}
	}
}
