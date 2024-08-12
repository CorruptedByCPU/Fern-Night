/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	#include	"../vfs.h"
	// library -------------------------------------------------------------
	#include	"font.h"
	#include	"interface.h"
	#include	"json.h"
	#include	"string.h"
	#include	"sys.h"
	// external ------------------------------------------------------------
	#include	"../../software/wm.h"
	//======================================================================

uint8_t lib_interface_string_window[] = "window";
uint8_t lib_interface_string_width[] = "width";
uint8_t lib_interface_string_height[] = "height";
uint8_t lib_interface_string_value[] = "value";
uint8_t lib_interface_string_x[] = "x";
uint8_t lib_interface_string_y[] = "y";
uint8_t	lib_interface_string_label[] = "label";
uint8_t	lib_interface_string_button[] = "button";
uint8_t	lib_interface_string_control[] = "control";
uint8_t	lib_interface_string_type[] = "type";
uint8_t lib_interface_string_close[] = "close";
uint8_t lib_interface_string_maximize[] = "maximize";
uint8_t lib_interface_string_minimize[] = "minimize";
uint8_t lib_interface_string_id[] = "id";
uint8_t lib_interface_string_align[] = "align";
uint8_t lib_interface_string_center[] = "center";
uint8_t lib_interface_string_right[] = "right";
uint8_t lib_interface_string_justify[] = "justify";
uint8_t	lib_interface_string_event[] = "event";
uint8_t lib_interface_string_list[] = "list";

void lib_interface_default_close( void ) {
	// end of execution
	lib_sys_exit();
}

void lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// prepare temporary message cache
	struct LIB_SYS_STRUCTURE_IPC ipc;

	// receive pending messages
	if( lib_sys_ipc_receive( (struct LIB_SYS_STRUCTURE_IPC *) &ipc, LIB_SYS_IPC_TYPE_MOUSE ) ) {
		// message properties
		struct LIB_SYS_STRUCTURE_IPC_MOUSE *mouse = (struct LIB_SYS_STRUCTURE_IPC_MOUSE *) &ipc.data;

		// pressed left mouse button?
		if( mouse -> status & LIB_SYS_device_mouse_status_left_mouse_button ) {
			// check which element is under cursor position
			uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
			while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
				// element properties
				struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

				// cursor overlaps this element? (check only if object is located under cursor)
				if( interface -> window_descriptor -> pointer_x >= properties -> x && interface -> window_descriptor -> pointer_y >= properties -> y && interface -> window_descriptor -> pointer_x < (properties -> x + properties -> width) && interface -> window_descriptor -> pointer_y < (properties -> y + properties -> height) ) {
					// execute event of element
					switch( properties -> type ) {
						case LIB_INTERFACE_ELEMENT_TYPE_mmc_close: {
							// execute function if exist
							if( properties -> event ) properties -> event();

							// done
							break;
						}

						case LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize: {
							// minimize window
							interface -> window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_minimize;

							// done
							break;
						}

						case LIB_INTERFACE_ELEMENT_TYPE_button: {
							// execute function if exist
							if( properties -> event ) properties -> event();

							// done
							break;
						}
					}
				}

				// next element from list
				e += properties -> size_byte;
			}
		}
	}

	//--------------------------------------------------------------------------------
	// "hover over elements"
	//--------------------------------------------------------------------------------
	lib_interface_hover( interface );
}

void lib_interface_hover( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// check every element of interface
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;

	// check which element is under cursor position
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// last event
		uint8_t previous = properties -> flags;

		// cursor overlaps this element? (check only if object is located under cursor)
		if( interface -> window_descriptor -> pointer_x >= properties -> x && interface -> window_descriptor -> pointer_y >= properties -> y && interface -> window_descriptor -> pointer_x < (properties -> x + properties -> width) && interface -> window_descriptor -> pointer_y < (properties -> y + properties -> height) )
			// mark as hovered
			properties -> flags |= LIB_INTERFACE_ELEMENT_FLAG_hover;
		else
			// mark as not hovered
			properties -> flags &= ~LIB_INTERFACE_ELEMENT_FLAG_hover;

		// if "event" changed
		if( properties -> flags != previous ) {
			// redraw element inside object
			if( properties -> type == LIB_INTERFACE_ELEMENT_TYPE_mmc_close || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_mmc_maximize || properties -> type == LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize ) lib_interface_element_mmc( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *) &element[ e ] );
			if( properties -> type == LIB_INTERFACE_ELEMENT_TYPE_button ) lib_interface_element_button( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) &element[ e ] );

			// update window content on screen
			interface -> window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_flush;
		}

		// next element from list
		e += properties -> size_byte;
	}
}

uintptr_t lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id ) {
	// check which element is under cursor position
	uint8_t *element = (uint8_t *) interface -> properties; uint64_t e = 0;
	while( element[ e ] != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		// element properties
		struct LIB_INTERFACE_STRUCTURE_ELEMENT *properties = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) &element[ e ];

		// element found?
		if( properties -> id == id ) return (uintptr_t) properties;

		// next element from list
		e += properties -> size_byte;
	}

	// element of ID not found
	return EMPTY;
}

void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// remove all whitespaces from json structure
	lib_json_cohesion( (uint8_t *) interface -> properties );

	// convert json interface to interface data
	lib_interface_convert( interface );

	// find control element for close function
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *close;
	if( (close = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, 1 ) ) ) close -> event = (void *) lib_interface_default_close;

	// if dimensions aquired
	if( interface -> width && interface -> height ) {
		// create window
		lib_interface_window( interface );

		// show interface elements
		lib_interface_draw( interface );
	}
}

void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// request properties of Framebuffer
	struct LIB_SYS_STRUCTURE_FRAMEBUFFER local_framebuffer = { EMPTY };
	lib_sys_framebuffer( (struct LIB_SYS_STRUCTURE_FRAMEBUFFER *) &local_framebuffer );

	// define properties of requesting window
	struct LIB_SYS_STRUCTURE_IPC local_message = { EMPTY };
	struct WM_STRUCTURE_IPC_WINDOW_create *data = (struct WM_STRUCTURE_IPC_WINDOW_create *) local_message.data;
	data -> ipc.type = LIB_SYS_IPC_TYPE_WINDOW_MANAGER;
	data -> ipc.function = WM_IPC_FUNCTION_create;
	data -> width = interface -> width;
	data -> height = interface -> height;

	// request window of definied size
	lib_sys_ipc_send( local_framebuffer.pid, (uint8_t *) data );

	// wait for answer from framebuffer manager
	while( TRUE ) {
		// retrieve messae from Window Manager
		if( lib_sys_ipc_receive( (struct LIB_SYS_STRUCTURE_IPC *) &local_message, LIB_SYS_IPC_TYPE_WINDOW_MANAGER ) ) {
			// properties of message
			struct WM_STRUCTURE_IPC *answer = (struct WM_STRUCTURE_IPC *) &local_message.data;

			// received new window properties?
			if( answer -> function == WM_IPC_FUNCTION_properties ) break;
		}
	}

	// answer properties
	struct WM_STRUCTURE_IPC_WINDOW_properties *properties = (struct WM_STRUCTURE_IPC_WINDOW_properties *) &local_message.data;
	interface -> window_descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) properties -> descriptor;

	// remember window id
	interface -> id = properties -> id;

	// set window base address
	interface -> base_address = (uint32_t *) ((uint64_t) interface -> window_descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));

	// clean window content
	lib_interface_clean( interface );

	// if window name is set
	if( interface -> length ) lib_interface_name( interface );

	// define properties of requesting window
	struct WM_STRUCTURE_IPC_WINDOW_rename *rename = (struct WM_STRUCTURE_IPC_WINDOW_rename *) local_message.data;
	rename -> ipc.type = LIB_SYS_IPC_TYPE_WINDOW_MANAGER;
	rename -> ipc.function = WM_IPC_FUNCTION_rename;
	rename -> id = interface -> id;
	rename -> length = interface -> length;
	for( uint16_t n = 0; n < rename -> length; n++ ) rename -> name[ n ] = interface -> name[ n ];

	// request window rename
	lib_sys_ipc_send( local_framebuffer.pid, (uint8_t *) rename );
}

void lib_interface_clean( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// fill window with default background color
	for( uint16_t y = 0; y < interface -> height; y++ )
		for( uint16_t x = 0; x < interface -> width; x++ )
			interface -> base_address[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;
}

void lib_interface_close( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// request properties of Framebuffer
	struct LIB_SYS_STRUCTURE_FRAMEBUFFER local_framebuffer = { EMPTY };
	lib_sys_framebuffer( (struct LIB_SYS_STRUCTURE_FRAMEBUFFER *) &local_framebuffer );

	// select window to close
	struct LIB_SYS_STRUCTURE_IPC local_message = { EMPTY };
	struct WM_STRUCTURE_IPC_WINDOW_close *data = (struct WM_STRUCTURE_IPC_WINDOW_close *) local_message.data;
	data -> ipc.type = LIB_SYS_IPC_TYPE_WINDOW_MANAGER;
	data -> ipc.function = WM_IPC_FUNCTION_close;
	data -> id = interface -> id;

	// request window of definied size
	lib_sys_ipc_send( local_framebuffer.pid, (uint8_t *) data );

	// release space aquired by window
	lib_sys_memory_release( (uintptr_t) interface -> window_descriptor, (uint64_t) sizeof( struct WM_STRUCTURE_DESCRIPTOR ) + ((interface -> width * interface -> height) << STATIC_VIDEO_DEPTH_shift) );
}

void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// properties of interface structure
	uint8_t *properties = (uint8_t *) lib_sys_memory_alloc( MACRO_PAGE_ALIGN_UP( lib_interface_length( (uint8_t *) interface -> properties ) ) ); uint64_t i = 0;

	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = lib_json( (uint8_t *) interface -> properties );

	// parse every element
	do {
		// window
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_window ) ) {
			// window properties
			struct LIB_JSON_STRUCTURE window = lib_json( (uint8_t *) json.value );

			// parse all keys
			do {
				// width
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_width ) ) interface -> width = window.value;
	
				// height
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_height ) ) interface -> height = window.value;

				// value
				if( lib_json_key( window, (uint8_t *) &lib_interface_string_value ) ) {
					// set string length
					interface -> length = window.length;
					if( interface -> length > LIB_INTERFACE_NAME_length ) interface -> length = LIB_INTERFACE_NAME_length;

					// set element name
					uint8_t *name = (uint8_t *) window.value;
					for( uint8_t i = 0; i < interface -> length; i++ ) interface -> name[ i ] = name[ i ];
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &window ) );
		}

		// control
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_control ) ) {
			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *) &properties[ i ];

			// control properties
			struct LIB_JSON_STRUCTURE control = lib_json( (uint8_t *) json.value );

			// default properties of label and button
			element -> mmc.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC );

			// parse all keys
			do {
				// x
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_x ) ) element -> mmc.x = control.value;

				// id
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_id ) ) element -> mmc.id = control.value;

				// y, width, height by default
				element -> mmc.y = EMPTY;
				element -> mmc.width = LIB_INTERFACE_HEADER_HEIGHT_pixel;
				element -> mmc.height = LIB_INTERFACE_HEADER_HEIGHT_pixel;

				// type
				if( lib_json_key( control, (uint8_t *) &lib_interface_string_type ) ) {
					// select proper element type
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) &lib_interface_string_close, control.length ) ) element -> mmc.type = LIB_INTERFACE_ELEMENT_TYPE_mmc_close;
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) &lib_interface_string_maximize, control.length ) ) element -> mmc.type = LIB_INTERFACE_ELEMENT_TYPE_mmc_maximize;
					if( lib_string_compare( (uint8_t *) control.value, (uint8_t *) &lib_interface_string_minimize, control.length ) ) element -> mmc.type = LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize;
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &control ) );

			// increase control elements amount
			interface -> mmc_length++;

			// change interface structure index
			i += element -> mmc.size_byte;
		}

		// element type
		uint8_t type = EMPTY;
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_label ) ) type = LIB_INTERFACE_ELEMENT_TYPE_label;
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_button ) ) type = LIB_INTERFACE_ELEMENT_TYPE_button;

		// label or button
		if( type ) {
			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) &properties[ i ];
	
			// label or button properties
			struct LIB_JSON_STRUCTURE label_or_button = lib_json( (uint8_t *) json.value );

			// default properties of label and button
			element -> label_or_button.type = type;
			element -> label_or_button.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON );

			// parse all keys
			do {
				// id
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_id ) ) element -> label_or_button.id = label_or_button.value;

				// x
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_x ) ) element -> label_or_button.x = label_or_button.value;

				// y
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_y ) ) element -> label_or_button.y = label_or_button.value;

				// width
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_width ) ) element -> label_or_button.width = label_or_button.value;

				// height
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_height ) ) element -> label_or_button.height = label_or_button.value;

				// event
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_event ) ) {
					// event field type is string?
					if( label_or_button.type == LIB_JSON_TYPE_string ) {
						// set length
						element -> command_length = label_or_button.length;

						// and name
						uint8_t *name = (uint8_t *) label_or_button.value;
						for( uint8_t i = 0; i < element -> command_length; i++ ) element -> command[ i ] = name[ i ];
					}
				}

				// align
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_align ) ) {
					// by default
					element -> label_or_button.flags = LIB_FONT_ALIGN_left;

					// center?
					if( lib_string_compare( (uint8_t *) label_or_button.value, (uint8_t *) &lib_interface_string_center, label_or_button.length ) )
						element -> label_or_button.flags = LIB_FONT_ALIGN_center;

					// right?
					if( lib_string_compare( (uint8_t *) label_or_button.value, (uint8_t *) &lib_interface_string_right, label_or_button.length ) )
						element -> label_or_button.flags = LIB_FONT_ALIGN_right;
				}

				// value
				if( lib_json_key( label_or_button, (uint8_t *) &lib_interface_string_value ) ) {
					// set length
					element -> length = label_or_button.length;
				
					// and name
					uint8_t *name = (uint8_t *) label_or_button.value;
					for( uint8_t i = 0; i < label_or_button.length; i++ ) element -> name[ i ] = name[ i ];
				}
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &label_or_button ) );

			// if label or button position is negative, align from other side of window
			if( element -> label_or_button.x < EMPTY ) element -> label_or_button.x += interface -> width - element -> label_or_button.width;
			if( element -> label_or_button.y < EMPTY ) element -> label_or_button.y += interface -> height - element -> label_or_button.height;
			
			// change interface structure index
			i += element -> label_or_button.size_byte;
		}

		// list
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_list ) ) {
			// element structure position
			struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *) &properties[ i ];

			// control properties
			struct LIB_JSON_STRUCTURE list = lib_json( (uint8_t *) json.value );

			// default properties of label and button
			element -> list.type = LIB_INTERFACE_ELEMENT_TYPE_list;
			element -> list.size_byte = sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST );

			// parse all keys
			do {
				// x
				if( lib_json_key( list, (uint8_t *) &lib_interface_string_x ) ) element -> list.x = list.value;

				// y
				if( lib_json_key( list, (uint8_t *) &lib_interface_string_y ) ) element -> list.y = list.value;

				// width
				if( lib_json_key( list, (uint8_t *) &lib_interface_string_width ) ) element -> list.width = list.value;

				// height
				if( lib_json_key( list, (uint8_t *) &lib_interface_string_height ) ) element -> list.height = list.value;
			// next key
			} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &list ) );

			// change interface structure index
			i += element -> list.size_byte;
		}
	// next key
	} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &json ) );

	// new properties
	interface -> properties = (uintptr_t) properties;
};

void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// limit name length to header width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, interface -> name, interface -> length ) > interface -> width - 7 - (LIB_INTERFACE_HEADER_HEIGHT_pixel * interface -> mmc_length) ) if( ! --interface -> length ) return;

 	// clear header
 	for( uint64_t y = 1; y < LIB_INTERFACE_HEADER_HEIGHT_pixel - 1; y++ )
 		for( uint64_t x = 1; x < interface -> width - 1 - (LIB_INTERFACE_HEADER_HEIGHT_pixel * interface -> mmc_length); x++ )
 			interface -> base_address[ (y * interface -> width) + x ] = LIB_INTERFACE_HEADER_COLOR_background;

	// print header
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &interface -> name, interface -> length, STATIC_COLOR_white, interface -> base_address + (3 * interface -> width) + 5, interface -> width, LIB_FONT_ALIGN_left );
}

uint64_t lib_interface_length( uint8_t *interface_json ) {
	// properties of JSON structure
	struct LIB_JSON_STRUCTURE json = lib_json( interface_json );

	// current length required by interface
	uint64_t length = EMPTY;

	// check every element
	do {
		// label of button
		if( lib_json_key( json, (uint8_t *) &lib_interface_string_label ) || lib_json_key( json, (uint8_t *) &lib_interface_string_button ) ) length += sizeof( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON );
	} while( lib_json_next( (struct LIB_JSON_STRUCTURE *) &json ) );

	// return calculated length of all interface elements
	return length + 1;	// with NULL terminator
}

void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// limit string length to element width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> length ) > element -> label_or_button.width ) if( ! --element -> length ) return;

	// compute absolute address of first pixel of element space
	uint32_t *pixel = interface -> base_address + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

	// fill element with background color
	for( uint16_t y = 0; y < element -> label_or_button.height; y++ )
		for( uint16_t x = 0; x < element -> label_or_button.width; x++ )
			pixel[ (y * interface -> width) + x ] = LIB_INTERFACE_COLOR_background;

	// vertical align of element content
	if( element -> label_or_button.height > LIB_FONT_HEIGHT_pixel ) pixel += ((element -> label_or_button.height - LIB_FONT_HEIGHT_pixel) >> STATIC_DIVIDE_BY_2_shift) * interface -> width;

	// horizontal align of element content
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_center ) pixel += element -> label_or_button.width >> STATIC_DIVIDE_BY_2_shift;
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_right ) pixel += element -> label_or_button.width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, element -> label_or_button.flags );
}

void lib_interface_element_button( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element ) {
	// compute absolute address of first pixel in window element space
	uint32_t *pixel = interface -> base_address + (element -> label_or_button.y * interface -> width) + element -> label_or_button.x;

	// choose color of background
	uint32_t background_color = LIB_INTERFACE_ELEMENT_BUTTON_COLOR_background;
	if( element -> label_or_button.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) background_color = LIB_INTERFACE_ELEMENT_BUTTON_COLOR_background_light;

	// fill element with background color
	for( uint16_t y = 0; y < element -> label_or_button.height; y++ )
		for( uint16_t x = 0; x < element -> label_or_button.width; x++ )
			pixel[ (y * interface -> width) + x ] = background_color;

	// limit string length to element width
	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> length ) > element -> label_or_button.width ) if( ! --element -> length ) return;

	// vertical align of element content
	uint16_t vertical_align = element -> label_or_button.height; if( vertical_align >= LIB_FONT_HEIGHT_pixel ) vertical_align -= LIB_FONT_HEIGHT_pixel;
	pixel += ((vertical_align >> STATIC_DIVIDE_BY_2_shift) * interface -> width);

	// horizontal align of element content
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_left ) pixel += LIB_INTERFACE_ELEMENT_BUTTON_padding;
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_center ) pixel += element -> label_or_button.width >> STATIC_DIVIDE_BY_2_shift;
	if( element -> label_or_button.flags & LIB_FONT_ALIGN_right ) pixel += element -> label_or_button.width;

	// display the content of element
	lib_font( LIB_FONT_FAMILY_ROBOTO, element -> name, element -> length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, element -> label_or_button.flags );
}

void lib_interface_element_mmc( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *element ) {
	// compute absolute address of button position in window element space
	uint32_t *pixel = interface -> base_address + ((((LIB_INTERFACE_HEADER_HEIGHT_pixel - LIB_INTERFACE_ELEMENT_MANAGEMENT_size) / 2) * interface -> width ) + element -> mmc.x + ((LIB_INTERFACE_HEADER_HEIGHT_pixel - LIB_INTERFACE_ELEMENT_MANAGEMENT_size) / 2));

	// choose background color
	uint32_t background_color = LIB_INTERFACE_HEADER_COLOR_background;
	if( element -> mmc.flags & LIB_INTERFACE_ELEMENT_FLAG_hover ) {
		if( element -> mmc.type == LIB_INTERFACE_ELEMENT_TYPE_mmc_close ) background_color = LIB_INTERFACE_ELEMENT_MMC_CLOSE_COLOR_background_hover;
		else background_color = LIB_INTERFACE_ELEMENT_BUTTON_COLOR_background_light;
	}

	// clear element space
	uint32_t *background = interface -> base_address + element -> mmc.x;
	for( uint8_t y = 0; y < LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint8_t x = 0; x < LIB_INTERFACE_HEADER_HEIGHT_pixel; x++ )
			background[ (y * interface -> width) + x ] = background_color;

	// choose element type
	switch( element -> mmc.type ) {	
		case LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize: {
			// display minimize window button
			for( uint64_t x = 0; x <= LIB_INTERFACE_ELEMENT_MANAGEMENT_size; x++ ) pixel[ (LIB_INTERFACE_ELEMENT_MANAGEMENT_size * interface -> width) + x ] = LIB_INTERFACE_COLOR_foreground;

			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_mmc_maximize: {
			// display window maximize button
			for( uint64_t y = 0; y <= LIB_INTERFACE_ELEMENT_MANAGEMENT_size; y++ ) {
				pixel[ y ] = LIB_INTERFACE_COLOR_foreground;
				pixel[ y * interface -> width ] = LIB_INTERFACE_COLOR_foreground;
				pixel[ (y * interface -> width) + LIB_INTERFACE_ELEMENT_MANAGEMENT_size ] = LIB_INTERFACE_COLOR_foreground;
				pixel[ (LIB_INTERFACE_ELEMENT_MANAGEMENT_size * interface -> width) + y ] = LIB_INTERFACE_COLOR_foreground;
			}
			// done
			break;
		}
		case LIB_INTERFACE_ELEMENT_TYPE_mmc_close: {
			// display close window button
			for( uint64_t y = 0; y <= LIB_INTERFACE_ELEMENT_MANAGEMENT_size; y++ ) {
				pixel[ y + (y * interface -> width) ] = LIB_INTERFACE_COLOR_foreground;
				pixel[ (LIB_INTERFACE_ELEMENT_MANAGEMENT_size - y) + (y * interface -> width) ] = LIB_INTERFACE_COLOR_foreground;
			}
			// done
			break;
		}
	}
}

void lib_interface_element_list_insert( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *element, uint8_t *name, uint8_t length );
void lib_interface_element_list( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *element ) {
	// fill element with background color
	uint32_t color = LIB_INTERFACE_ELEMENT_LIST_COLOR_background0;
	for( uint16_t y = element -> list.y; y < element -> list.height + LIB_INTERFACE_HEADER_HEIGHT_pixel; y++ )
		for( uint16_t x = element -> list.x; x < element -> list.width; x++ ) {
			// choose background color
			if( ! (y % LIB_INTERFACE_ELEMENT_LIST_ENTRY_HEIGHT_pixel) ) {
				if( color == LIB_INTERFACE_ELEMENT_LIST_COLOR_background0 ) color = LIB_INTERFACE_ELEMENT_LIST_COLOR_background1;
				else color = LIB_INTERFACE_ELEMENT_LIST_COLOR_background0;
			}

			interface -> base_address[ (y * interface -> width) + x ] = color;
		}

	// assing space for entries if not available yet
	if( ! element -> entry ) element -> entry = (struct LIB_INTERFACE_STRUCTURE_LIST *) malloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_INTERFACE_STRUCTURE_LIST ) * (element -> entry_count + 1) ) );

	// debug
	uint8_t test[] = "test entry.txt";
	lib_interface_element_list_insert( element, (uint8_t *) &test, sizeof( test ) - 1 );
	lib_interface_element_list_insert( element, (uint8_t *) &test, sizeof( test ) - 1 );
	lib_interface_element_list_insert( element, (uint8_t *) &test, sizeof( test ) - 1 );
	lib_interface_element_list_insert( element, (uint8_t *) &test, sizeof( test ) - 1 );
	lib_interface_element_list_insert( element, (uint8_t *) &test, sizeof( test ) - 1 );

	// compute absolute address of first pixel of element space
	uint32_t *pixel = interface -> base_address + ((element -> list.y + ((LIB_INTERFACE_ELEMENT_LIST_ENTRY_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) >> STATIC_DIVIDE_BY_2_shift)) * interface -> width) + element -> list.x + 5;

	// show entries
	for( uint64_t i = 0; i < element -> entry_count; i++ ) {
		// display the content of element
		lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, element -> entry[ i ].name, element -> entry[ i ].length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, interface -> width, LIB_FONT_ALIGN_left );

		// next entry position
		pixel += (LIB_INTERFACE_ELEMENT_LIST_ENTRY_HEIGHT_pixel * interface -> width);
	}
}

void lib_interface_element_list_insert( struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *element, uint8_t *name, uint8_t length ) {
	// find an empty entry
	uint64_t i = 0; for( ; i < element -> entry_count; i++ )
		if( ! element -> entry[ i ].length ) break;

	// entry found?
	if( i == element -> entry_count )	// no
		// extend list of entries
		element -> entry = (struct LIB_INTERFACE_STRUCTURE_LIST *) realloc( (uintptr_t) element -> entry, MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_INTERFACE_STRUCTURE_LIST ) * (element -> entry_count + 1) ) );

	// insert new entry
	element -> entry[ i ].length = length;
	for( uint8_t n = 0; n < LIB_INTERFACE_ELEMENT_LIST_ENTRY_NAME_length; n++ ) element -> entry[ i ].name[ n ] = name[ n ];

	// inserted
	element -> entry_count++;
}

void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface ) {
	// first element properties
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) interface -> properties;

	// process all interface elements
	while( element -> type != LIB_INTERFACE_ELEMENT_TYPE_null ) {
		switch( element -> type ) {
			case LIB_INTERFACE_ELEMENT_TYPE_mmc_close: { lib_interface_element_mmc( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_mmc_maximize: { lib_interface_element_mmc( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize: { lib_interface_element_mmc( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_label: { lib_interface_element_label( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_button: { lib_interface_element_button( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) element ); break; }
			case LIB_INTERFACE_ELEMENT_TYPE_list: { lib_interface_element_list( interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST *) element ); break; }
		}
	
		// next element properties
		element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) ((uint64_t) element + element -> size_byte);
	}
}