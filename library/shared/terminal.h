/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	LIB_TERMINAL
	#define	LIB_TERMINAL

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"font.h"
	//======================================================================

	#define	LIBRARY_TERMINAL_BACKGROUND_color	STATIC_COLOR_black

	#define LIBRARY_TERMINAL_TAB_length		8

	#define	LIBRARY_TERMINAL_FLAG_clean		0b00000001	// clean space under character

	struct LIB_TERMINAL_STRUCTURE {
		uint16_t	width;
		uint16_t	height;
		uint32_t	*base_address;
		uint32_t	scanline_pixel;
		uint32_t	scanline_line;
		uint32_t	*pointer;
		uint32_t	color_foreground;
		uint32_t	color_background;
		uint8_t		alpha;
		uint8_t		flags;
		uint16_t	width_char;
		uint16_t	height_char;
		uint16_t	scanline_char;
		uint16_t	cursor_x;
		uint16_t	cursor_y;
		uint64_t	lock;
	};

	struct	LIBRARY_TERMINAL_CURSOR {
		uint16_t	x;
		uint16_t	y;
	} __attribute__( (packed) );

	void lib_terminal( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_char( struct LIB_TERMINAL_STRUCTURE *terminal, uint8_t ascii );
	void lib_terminal_char_drain( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_disable( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_enable( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_set( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_switch( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_drain( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_drain_line( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_scroll_up( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_string( struct LIB_TERMINAL_STRUCTURE *terminal, uint8_t *string, uint64_t length );
	void lib_terminal_value( struct LIB_TERMINAL_STRUCTURE *terminal, uint64_t value, uint8_t base );
#endif