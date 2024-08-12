/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_clock() {
	// // time changed?
	// uint64_t time = lib_sys_time();
	// if( time != wm_clock_last ) {	// yes
	// 	// update timestamp
	// 	wm_clock_last = time;

	// 	// get current time
	// 	uint8_t hour = (uint8_t) (time >> STATIC_MOVE_HIGH_TO_AX_shift);
	// 	uint8_t minutes = (uint8_t) (time >> STATIC_MOVE_HIGH_TO_AL_shift);
	// 	uint8_t seconds = (uint8_t) time;

	// 	// semicolon animation
	// 	if( ! (seconds % 2) ) taskbar_element_clock -> name[ 2 ] = STATIC_ASCII_COLON; else taskbar_element_clock -> name[ 2 ] = STATIC_ASCII_SPACE;

	// 	// hour
	// 	if( hour < STATIC_NUMBER_SYSTEM_decimal ) { taskbar_element_clock -> name[ 0 ] = STATIC_ASCII_SPACE; lib_integer_to_string( hour, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taskbar_element_clock -> name[ 1 ] ); }
	// 	else lib_integer_to_string( hour, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) taskbar_element_clock -> name );

	// 	// minutes
	// 	if( minutes < STATIC_NUMBER_SYSTEM_decimal ) { taskbar_element_clock -> name[ 3 ] = STATIC_ASCII_DIGIT_0; lib_integer_to_string( minutes, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taskbar_element_clock -> name[ 4 ] ); }
	// 	else lib_integer_to_string( minutes, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taskbar_element_clock -> name[ 3 ] );

	// 	// limit string length to element width
	// 	while( lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, taskbar_element_clock -> name, taskbar_element_clock -> length ) > taskbar_element_clock -> label_or_button.width ) if( ! --taskbar_element_clock -> length ) return;

	// 	// compute absolute address of first pixel of element space
	// 	uint32_t *pixel = taskbar_clock.base_address + (taskbar_element_clock -> label_or_button.y * taskbar_clock.width) + taskbar_element_clock -> label_or_button.x;

	// 	// fill element with background color
	// 	for( uint16_t y = 0; y < taskbar_element_clock -> label_or_button.height; y++ )
	// 		for( uint16_t x = 0; x < taskbar_element_clock -> label_or_button.width; x++ )
	// 			pixel[ (y * taskbar_clock.width) + x ] = WM_TASKBAR_BACKGROUND_COLOR_default;

	// 	// vertical align of element content
	// 	if( taskbar_element_clock -> label_or_button.height > LIB_FONT_HEIGHT_pixel ) pixel += ((taskbar_element_clock -> label_or_button.height - LIB_FONT_HEIGHT_pixel) >> STATIC_DIVIDE_BY_2_shift) * taskbar_clock.width;

	// 	// horizontal align of element content
	// 	pixel += taskbar_element_clock -> label_or_button.width >> STATIC_DIVIDE_BY_2_shift;

	// 	// display the content of element
	// 	lib_font( LIB_FONT_FAMILY_ROBOTO, taskbar_element_clock -> name, taskbar_element_clock -> length, LIB_INTERFACE_COLOR_foreground, (uint32_t *) pixel, taskbar_clock.width, taskbar_element_clock -> label_or_button.flags );

	// 	// refresh taskbar on screen
	// 	wm_object_taskbar -> flags |= WM_OBJECT_FLAG_flush;
	// }
}