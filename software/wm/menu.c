/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

// uint8_t menu_json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( thread_menu_json, "software/wm/menu.json" );

struct LIB_INTERFACE_STRUCTURE thread_menu_interface = { (uintptr_t) &thread_menu_json };

void wm_thread_menu_reload() {
       	// initialize interface of menu application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &thread_menu_interface );

	// locate object
	wm_object_menu = wm_object_by_id( thread_menu_interface.id );

	// mark object as menu and fragile
	wm_object_menu -> flags = WM_OBJECT_FLAG_hide | WM_OBJECT_FLAG_menu | WM_OBJECT_FLAG_fragile | WM_OBJECT_FLAG_flush;
}

void wm_thread_menu() {
	// sad hack :|
	__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

	// create new menu window
	wm_thread_menu_reload();

	// main loop
	while( TRUE ) {
		// prepare temporary message cache
		struct LIB_SYS_STRUCTURE_IPC ipc;

		// receive pending messages
		if( lib_sys_ipc_receive( (struct LIB_SYS_STRUCTURE_IPC *) &ipc, LIB_SYS_IPC_TYPE_MOUSE ) ) {
			// message properties
			struct LIB_SYS_STRUCTURE_IPC_MOUSE *mouse = (struct LIB_SYS_STRUCTURE_IPC_MOUSE *) &ipc.data;

			// pressed left mouse button?
			if( mouse -> status & LIB_SYS_DRIVER_MOUSE_STATUS_left_mouse_button ) {
				// check which element is under cursor position
				uint8_t *element = (uint8_t *) thread_menu_interface.properties; uint64_t e = 0;
				while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
					// element properties
					struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) &element[ e ];

					// cursor overlaps this element? (check only if object is located under cursor)
					if( thread_menu_interface.window_descriptor -> pointer_x >= properties -> label_or_button.x && thread_menu_interface.window_descriptor -> pointer_y >= properties -> label_or_button.y && thread_menu_interface.window_descriptor -> pointer_x < (properties -> label_or_button.x + properties -> label_or_button.width) && thread_menu_interface.window_descriptor -> pointer_y < (properties -> label_or_button.y + properties -> label_or_button.height) ) {
						// execute event of element
						switch( properties -> label_or_button.type ) {
							case LIB_INTERFACE_ELEMENT_TYPE_button: {
								// execute process described in event field
								lib_sys_exec( (uint8_t *) &properties -> command, properties -> command_length, LIB_SYS_STREAM_FLOW_out_to_parent_out );

								// done
								break;
							}
						}
					}

					// next element from list
					e += properties -> label_or_button.size_byte;
				}
			}
		}

		//--------------------------------------------------------------------------------
		// "hover over elements"
		//--------------------------------------------------------------------------------
		lib_interface_hover( (struct LIB_INTERFACE_STRUCTURE *) &thread_menu_interface );

		// // right mouse button pressed?
		// if( wm_mouse.status & LIB_SYS_DRIVER_MOUSE_STATUS_right_mouse_button ) {
		// 	// right mouse button held down?
		// 	if( ! wm_mouse_button_right_semaphore ) {	// no
		// 		// save key state
		// 		wm_mouse_button_right_semaphore = TRUE;

		// 		// get a pointer to the object under the cursor
		// 		struct WM_STRUCTURE_OBJECT *object = wm_object_find( wm_mouse.x, wm_mouse.y, FALSE );

		// 		// under cursor position lays workbench window?
		// 		if( object -> base_address == wm_object_workbench -> base_address ) {
		// 			// menu window already visible?
		// 			if( wm_object_menu -> flags & WM_OBJECT_FLAG_visible ) continue;

		// 			// update position of menu window
		// 			wm_object_menu -> x = wm_mouse.x;
		// 			wm_object_menu -> y = wm_mouse.y;

		// 			// menu window might be covered?
		// 			if( wm_object_menu -> x > wm_object_workbench -> width - wm_object_menu -> width ) wm_object_menu -> x -= wm_object_menu -> width;
		// 			if( wm_object_menu -> y + wm_object_menu -> height > wm_object_workbench -> height - WM_OBJECT_TASKBAR_HEIGHT_pixel ) wm_object_menu -> y -= wm_object_menu -> height;

		// 			// move menu object on top of everything else (except artiber ;)
		// 			wm_object_move_up( wm_object_menu );

		// 			// make menu window visible
		// 			wm_object_menu -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;	// and show it (flush)
		// 		}
		// 	}
		// } else
		// 	// release semaphore
		// 	wm_mouse_button_right_semaphore = FALSE;
	}
}