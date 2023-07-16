/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#define	CONSOLE_WINDOW_width_char	80
#define	CONSOLE_WINDOW_height_char	25
#define	CONSOLE_WINDOW_width_pixel	((LIB_FONT_WIDTH_pixel * CONSOLE_WINDOW_width_char) + 2)
#define	CONSOLE_WINDOW_height_pixel	((LIB_FONT_HEIGHT_pixel * CONSOLE_WINDOW_height_char) + LIB_INTERFACE_HEADER_HEIGHT_pixel + 1)

void close( void );