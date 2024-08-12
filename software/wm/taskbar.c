/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// uint8_t menu_json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( thread_taskbar_json, "software/wm/taskbar.json" );

struct LIB_INTERFACE_STRUCTURE wm_thread_taskbar_interface = { (uintptr_t) &thread_taskbar_json };

struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *wm_thread_taskbar_element_menu;
struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *wm_thread_taskbar_element_clock;

uint8_t wm_thread_taskbar_window_count;
uint64_t wm_thread_taskbar_width_entry;
uint64_t wm_thread_taskbar_width;

volatile uint8_t wm_thread_taskbar_menu_semaphore = FALSE;

uint8_t wm_taskbar_clock_semaphore = FALSE;
uint64_t wm_taskbar_clock_last = EMPTY;

void wm_thread_taskbar_init() {
	// mark taskbar object as own
	wm_object_taskbar -> pid = lib_sys_task_pid();

       	// initialize taskbar elements
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &wm_thread_taskbar_interface );

	// fill in object properties
	wm_thread_taskbar_interface.width = wm_object_taskbar -> width;
	wm_thread_taskbar_interface.height = wm_object_taskbar -> height;
	wm_thread_taskbar_interface.base_address = wm_object_taskbar -> base_address;
	wm_thread_taskbar_interface.window_descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) wm_object_taskbar -> base_address - (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

	// menu button
	wm_thread_taskbar_element_menu = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &wm_thread_taskbar_interface, 1 );

	// clock label
	wm_thread_taskbar_element_clock = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &wm_thread_taskbar_interface, 2 );
	wm_thread_taskbar_element_clock -> label_or_button.x = wm_object_taskbar -> width - wm_thread_taskbar_element_clock -> label_or_button.width;	// default position at X axis

	// taskbar object visible
	wm_object_taskbar -> flags |= WM_OBJECT_FLAG_visible;

	// fist run
	wm_taskbar_semaphore = TRUE;
}

void wm_thread_taskbar_entry( struct WM_STRUCTURE_OBJECT *object, uint16_t x, uint16_t width ) {
	// select proper background color for entry
	uint32_t color = WM_TASKBAR_BACKGROUND_COLOR_invisible;	// default

	// or
	if( object -> flags & WM_OBJECT_FLAG_visible ) color = WM_TASKBAR_BACKGROUND_COLOR_visible;	// visible

	// fill element with default background color
	for( uint16_t row = 0; row < wm_object_taskbar -> height; row++ )
		for( uint16_t col = x; col < x + width - 1; col++ )
			wm_object_taskbar -> base_address[ (row * wm_object_taskbar -> width) + col ] = color;

	// mark active window
	if( object == wm_object_active ) for( uint16_t row = wm_object_taskbar -> height - 2; row < wm_object_taskbar -> height; row++ )
		for( uint16_t col = x; col < x + width - 1; col++ )
			wm_object_taskbar -> base_address[ (row * wm_object_taskbar -> width) + col ] = STATIC_COLOR_green;

	// element name
	color = STATIC_COLOR_gray_light; if( object == wm_object_active ) color = STATIC_COLOR_white;
	lib_font( LIB_FONT_FAMILY_ROBOTO, object -> name, object -> length, color, wm_object_taskbar -> base_address + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + x + (LIB_FONT_HEIGHT_pixel / 2), wm_object_taskbar -> width, LIB_FONT_ALIGN_left );
}

void wm_thread_taskbar_list() {
	// first window element position
	uint16_t x_entry = WM_OBJECT_TASKBAR_ELEMENT_X;

	// length of taskbar window list in pixels
	uint32_t wm_thread_taskbar_width = (wm_object_taskbar -> width - wm_thread_taskbar_element_clock -> label_or_button.width) - WM_OBJECT_TASKBAR_ELEMENT_X;

	// fill taskbar window with default color
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = 0; x < wm_object_taskbar -> width; x++ )
			wm_object_taskbar -> base_address[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BACKGROUND_COLOR_default;

	// show menu buton on taskbar
	uint8_t test[3] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), 0xFFFFFFFF, wm_object_taskbar -> base_address + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + (wm_thread_taskbar_element_menu -> label_or_button.width >> STATIC_DIVIDE_BY_2_shift), wm_object_taskbar -> width, LIB_FONT_ALIGN_center );

	// amount of windows on taskbar and its object pointers
	wm_thread_taskbar_window_count = EMPTY;
	for( uint8_t i = 0; i < WM_OBJECT_limit; i++ ) if( wm_object_base_address[ i ].flags && wm_object_base_address[ i ].pid != wm_pid_number && ! (wm_object_base_address[ i ].flags & (WM_OBJECT_FLAG_menu | WM_OBJECT_FLAG_arbiter)) ) wm_taskbar_elements[ wm_thread_taskbar_window_count++ ] = (uintptr_t) &wm_object_base_address[ i ];

	// are there open windows?
	if( wm_thread_taskbar_window_count ) {
		// default with of every window on taskbar
		wm_thread_taskbar_width_entry = wm_thread_taskbar_width / wm_thread_taskbar_window_count;
		uint8_t align = wm_thread_taskbar_width % wm_thread_taskbar_width_entry;
		if( wm_thread_taskbar_width_entry > WM_TASKBAR_ELEMENT_WIDTH_pixel ) wm_thread_taskbar_width_entry = WM_TASKBAR_ELEMENT_WIDTH_pixel;

		// for every window
		for( uint8_t e = 0; e < wm_thread_taskbar_window_count; e++ ) {
			// last window on taskbar?
			if( e == wm_thread_taskbar_window_count - 1 ) wm_thread_taskbar_width_entry += align;	// align to width

			// show on taskbar list
			wm_thread_taskbar_entry( (struct WM_STRUCTURE_OBJECT *) wm_taskbar_elements[ e ], x_entry, wm_thread_taskbar_width_entry );

			// next window position
			x_entry += wm_thread_taskbar_width_entry;
		}
	}

	// taskbar window list updated
	wm_taskbar_semaphore = FALSE;

	// redraw clock
	wm_taskbar_clock_semaphore = TRUE;
}

void wm_thread_taskbar_clock( uint64_t time ) {
	// update timestamp
	wm_taskbar_clock_last = time;

	// get current time
	uint8_t hour = (uint8_t) (time >> STATIC_MOVE_HIGH_TO_AX_shift);
	uint8_t minutes = (uint8_t) (time >> STATIC_MOVE_HIGH_TO_AL_shift);
	uint8_t seconds = (uint8_t) time;

	// semicolon animation
	if( ! (seconds % 2) ) wm_thread_taskbar_element_clock -> name[ 2 ] = STATIC_ASCII_COLON; else wm_thread_taskbar_element_clock -> name[ 2 ] = STATIC_ASCII_SPACE;

	// hour
	if( hour < STATIC_NUMBER_SYSTEM_decimal ) { wm_thread_taskbar_element_clock -> name[ 0 ] = STATIC_ASCII_SPACE; lib_integer_to_string( hour, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &wm_thread_taskbar_element_clock -> name[ 1 ] ); }
	else lib_integer_to_string( hour, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) wm_thread_taskbar_element_clock -> name );

	// minutes
	if( minutes < STATIC_NUMBER_SYSTEM_decimal ) { wm_thread_taskbar_element_clock -> name[ 3 ] = STATIC_ASCII_DIGIT_0; lib_integer_to_string( minutes, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &wm_thread_taskbar_element_clock -> name[ 4 ] ); }
	else lib_integer_to_string( minutes, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &wm_thread_taskbar_element_clock -> name[ 3 ] );

	// limit string length to element width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, wm_thread_taskbar_element_clock -> name, wm_thread_taskbar_element_clock -> length ) > wm_thread_taskbar_element_clock -> label_or_button.width ) if( ! --wm_thread_taskbar_element_clock -> length ) return;

	// compute absolute address of first pixel of element space
	uint32_t *pixel = wm_object_taskbar -> base_address + (wm_thread_taskbar_element_clock -> label_or_button.y * wm_object_taskbar -> width) + wm_thread_taskbar_element_clock -> label_or_button.x;

	// fill element with background color
	for( uint16_t y = 0; y < wm_thread_taskbar_element_clock -> label_or_button.height; y++ )
		for( uint16_t x = 0; x < wm_thread_taskbar_element_clock -> label_or_button.width; x++ )
			pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BACKGROUND_COLOR_default;

	// vertical align of element content
	if( wm_thread_taskbar_element_clock -> label_or_button.height > LIB_FONT_HEIGHT_pixel ) pixel += ((wm_thread_taskbar_element_clock -> label_or_button.height - LIB_FONT_HEIGHT_pixel) >> STATIC_DIVIDE_BY_2_shift) * wm_object_taskbar -> width;

	// horizontal align of element content
	pixel += wm_thread_taskbar_element_clock -> label_or_button.width >> STATIC_DIVIDE_BY_2_shift;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, wm_thread_taskbar_element_clock -> name, wm_thread_taskbar_element_clock -> length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, wm_object_taskbar -> width, wm_thread_taskbar_element_clock -> label_or_button.flags );

	// reload taskbar object
	wm_object_taskbar -> flags |= WM_OBJECT_FLAG_flush;

	// clock parsed
	wm_taskbar_clock_semaphore = FALSE;
}

void wm_thread_taskbar() {
	// sad hack :|
	__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

	// initialize interface of menu application
	wm_thread_taskbar_init();

	// main loop
	while( TRUE ) {
		// prepare temporary message cache
		struct LIB_SYS_STRUCTURE_IPC ipc;

		// message properties
		struct LIB_SYS_STRUCTURE_IPC_MOUSE *mouse = (struct LIB_SYS_STRUCTURE_IPC_MOUSE *) &ipc.data;

		// receive pending messages
		if( lib_sys_ipc_receive( (struct LIB_SYS_STRUCTURE_IPC *) &ipc, LIB_SYS_IPC_TYPE_MOUSE ) ) {
			// pressed left mouse button?
			if( mouse -> status & LIB_SYS_device_mouse_status_left_mouse_button ) {
				// cursor over taskbar menu button?
				if( wm_mouse.x < WM_OBJECT_TASKBAR_HEIGHT_pixel ) {
					// menu window invisible?
					if( ! (wm_object_menu -> flags & WM_OBJECT_FLAG_visible) ) {
						// update position of menu window
						wm_object_menu -> x = EMPTY;	// default position at X axis
						wm_object_menu -> y = wm_object_taskbar -> y - wm_object_menu -> height;

						// move menu object on top of everything else (except artiber ;)
						wm_object_move_up( wm_object_menu );

						// set as active
						wm_object_active = wm_object_menu;

						// make menu window visible
						wm_object_menu -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;	// and show it (flush)

						// do not close menu window yet
						wm_thread_taskbar_menu_semaphore = TRUE;
					}
				}

				// hide menu window?
				if( ! wm_thread_taskbar_menu_semaphore ) {
					// hide object
					wm_object_menu -> flags &= ~WM_OBJECT_FLAG_visible;

					// redraw object position
					wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_menu, FALSE );

					// find next active window
					wm_object_activate();
				} else
					// yes
					wm_thread_taskbar_menu_semaphore = FALSE;

				// if there are windows on the list
				if( wm_thread_taskbar_window_count ) {
					// cursor position in range of any entry?
					if( wm_mouse.x >= WM_OBJECT_TASKBAR_ELEMENT_X && (wm_mouse.x - WM_OBJECT_TASKBAR_ELEMENT_X) / wm_thread_taskbar_width_entry < wm_thread_taskbar_window_count ) {
						// properties of selected entry
						struct WM_STRUCTURE_OBJECT *object = (struct WM_STRUCTURE_OBJECT *) wm_taskbar_elements[ (wm_mouse.x - WM_OBJECT_TASKBAR_ELEMENT_X) / wm_thread_taskbar_width_entry ];

						if( object == wm_object_active ) {
							// hide object
							object -> flags ^= WM_OBJECT_FLAG_visible;
							object -> flags ^= WM_OBJECT_FLAG_hide;

							// and select next one
							wm_object_activate();
						} else {
							// mark as active
							wm_object_active = object;

							// move the object to the top position in the list
							wm_object_move_up( object );

							// redraw the object
							object -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush;

							// the cursor pointer may be obscured, redraw again
							wm_object_cursor -> flags |= WM_OBJECT_FLAG_flush;
						}
					}
				}

				// reload taskbar
				wm_taskbar_semaphore = TRUE;
			}
		}

		// one of the objects was modified?
		if( wm_taskbar_semaphore ) wm_thread_taskbar_list();

		// retrieve current time
		uint64_t clock = lib_sys_time();

		// time changed?
		if( wm_taskbar_clock_semaphore || clock != wm_taskbar_clock_last ) wm_thread_taskbar_clock( clock );
	}
}