/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// driver --------------------------------------------------------------
	#include	"../kernel/driver/ps2.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/font.h"
	#include	"../library/shared/integer.h"
	#include	"../library/shared/interface.c"
	#include	"../library/shared/random.h"
	#include	"../library/shared/rgl.h"
	#include	"../library/shared/sys.h"
	#include	"../library/shared/string.h"
	// external ------------------------------------------------------------
	#include	"wm.h"
	//----------------------------------------------------------------------
	// variables, functions, routines
	//----------------------------------------------------------------------
	#include	"taris/config.h"
	#include	"taris/data.c"
	#include	"taris/init.c"
	#include	"taris/collision.c"
	#include	"taris/random.c"
	#include	"taris/draw.c"
	//======================================================================

void close( void ) {
	// end of execution
	lib_sys_exit();
}

void statistics( uint64_t points, uint64_t lines, uint64_t level ) {
	// points
	taris_points -> length = lib_integer_to_string( points, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taris_points -> name[ 0 ] );
	lib_interface_element_label( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, taris_points );

	// lines
	taris_lines -> length = lib_integer_to_string( lines, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taris_lines -> name[ 0 ] );
	lib_interface_element_label( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, taris_lines );

	// level
	taris_level -> length = lib_integer_to_string( level, STATIC_NUMBER_SYSTEM_decimal, (uint8_t *) &taris_level -> name[ 0 ] );
	lib_interface_element_label( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, taris_level );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize gameplay
	taris_init();

	// reset statistics
	uint64_t points = 0;
	uint64_t lines = 0;
	uint64_t level = 0;

	// restart game loop
	while( TRUE ) {
		// draw a new block
		taris_random();

		// removed lines at collision
		uint8_t removed = 0;

		// playgame loop
		while( taris_play ) {
			// move block one level lower
			taris_brick_selected_y++;

			// check collision
			if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) {
				// collision at startpoint?
				if( taris_brick_selected_y == 0 ) taris_play = FALSE;
				else {
					// undo block movement
					taris_brick_selected_y--;

					// for every line of brick
					for( uint8_t n = 0; n < TARIS_BRICK_WIDTH_bit; n++ ) {
						// sync with playground
						uint16_t block_line = ((taris_brick_selected >> (n * STATIC_BYTE_HALF_SIZE_bit)) & STATIC_BYTE_HALF_mask) << taris_brick_selected_x;
						if( block_line ) taris_playground[ taris_brick_selected_y + n ] |= block_line;

						// and preserve color
						for( uint8_t c = 0; c < TARIS_BRICK_WIDTH_bit; c++ ) if( taris_brick_selected >> (n * STATIC_BYTE_HALF_SIZE_bit) & 1 << c ) taris_playground_color[ ((taris_brick_selected_y + n) * TARIS_PLAYGROUND_WIDTH_bit) + (taris_brick_selected_x + c) ] = taris_brick_selected_id;
					}

					// remove every full line
					for( uint8_t i = 1; i < TARIS_PLAYGROUND_HEIGHT_brick - 1; i++ ) if( taris_playground[ i ] == TARIS_PLAYGROUND_full ) {
						// and remove it
						for( uint8_t j = i; j >= 1; j-- ) {
							// line
							taris_playground[ j ] = taris_playground[ j - 1 ];

							// and color
							for( uint8_t c = 0; c < TARIS_PLAYGROUND_WIDTH_bit; c++ )
								taris_playground_color[ (j * TARIS_PLAYGROUND_WIDTH_bit) + c ] = taris_playground_color[ ((j - 1) * TARIS_PLAYGROUND_WIDTH_bit) + c ];
						}

						// removed lines
						removed++;
					}
				}

				// if rmeoved some lines
				if( removed ) {
					// increase overall lines
					lines += removed;

					// calculate scored points
					points += taris_score[ removed - 1 ] * (level + 1);

					// increase difficulty?
					if( lines >= taris_difficult[ level ] ) level++;
				}

				// next block
				break;
			}

			// wait for next movement
			uint64_t wait = lib_sys_uptime() + taris_speed[ level ];
			do {
				// check for events from interface
				lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface );

				// recieve key
				uint16_t key = getkey();

				// move left?
				if( key == DRIVER_PS2_KEYBOARD_PRESS_ARROW_LEFT && ! taris_collision( taris_brick_selected_x - 1, taris_brick_selected_y ) ) taris_brick_selected_x--;
				// move right?
				if( key == DRIVER_PS2_KEYBOARD_PRESS_ARROW_RIGHT && ! taris_collision( taris_brick_selected_x + 1, taris_brick_selected_y ) ) taris_brick_selected_x++;
				// rotate left?
				if( key == STATIC_ASCII_LETTER_z ) {
					taris_brick_selected = (taris_brick_selected >> (64 - 16)) | (taris_brick_selected << 16);
					if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) taris_brick_selected = (taris_brick_selected >> 16) | (taris_brick_selected << (64 - 16));
				}
				// rotate right?
				if( key == STATIC_ASCII_LETTER_x ) {
					taris_brick_selected = (taris_brick_selected >> 16) | (taris_brick_selected << (64 - 16));
					if( taris_collision( taris_brick_selected_x, taris_brick_selected_y ) ) taris_brick_selected = (taris_brick_selected >> (64 - 16)) | (taris_brick_selected << 16);
				}
				// hard drop?
				if( key == DRIVER_PS2_KEYBOARD_PRESS_ARROW_UP ) {
					// move block down until collision
					while( ! taris_collision( taris_brick_selected_x, taris_brick_selected_y + 1 ) ) taris_brick_selected_y++;

					// next block
					break;
				}
				// end of exeution?
				if( key == STATIC_ASCII_LETTER_q ) return 0;	// yes

				// show current game status
				taris_draw();
			} while( wait > lib_sys_uptime() );
		}

		// if end of game
		if( ! taris_play ) {
			// show end of game
			lib_interface_element_label( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) &game_over );

			// reset statistics
			points = 0;
			lines = 0;
			level = 0;

			// wait for restart of game
			while( getkey() != STATIC_ASCII_SPACE ) {
				// check for events from interface
				lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface );
			};

			// clean up playground
			for( uint8_t i = 0; i < TARIS_PLAYGROUND_HEIGHT_brick - 1; i++ )
				taris_playground[ i ] = TARIS_PLAYGROUND_empty;

			// start new round
			taris_play = TRUE;
		}

		// update fields
		statistics( points, lines, level );
	}

	// end of execution
	return 0;
}