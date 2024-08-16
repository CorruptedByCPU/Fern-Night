/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/interface.c"
	#include	"../library/shared/terminal.c"
	#include	"../library/shared/string.h"
	#include	"../library/shared/sys.h"
	// external ------------------------------------------------------------
	#include	"wm.h"
	#include	"console.h"
	// console -------------------------------------------------------------
	#include	"console/config.h"
	#include	"console/data.c"
	#include	"console/init.c"
	//======================================================================

void close( void ) {
	// end of execution
	lib_sys_exit();
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize console
	if( console_init( argc, argv ) ) return 0;

	// main loop
	while( TRUE ) {
		// execute child again, if exited
		if( ! lib_sys_task_status( console_pid_child ) ) {
			// if we are not the captain and that wasn't initial child PID
			if( ! console_the_master_of_puppets && console_pid_child != -1 ) lib_sys_exit();

                        // clear terminal space
			lib_terminal_drain( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

                        // show hello
                        if( console_the_master_of_puppets ) lib_terminal_string( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, (uint8_t *) "Welcome!\n", 9 );

                        // args available?
                        if( argc > 1 ) {
                                // execute specified software
                                uint8_t soft[ lib_string_length( argv[ 1 ] ) ];
                                for( uint64_t i = 0; i < lib_string_length( argv[ 1 ] ); i++ ) soft[ i ] = argv[ 1 ][ i ];

                                // execute
                                if( ! (console_pid_child = lib_sys_exec( (uint8_t *) &soft, sizeof( soft ), LIB_SYS_STREAM_FLOW_out_to_parent_in )) ) return 0;
                        } else
                                // execute shell
                                if( ! (console_pid_child = lib_sys_exec( (uint8_t *) &console_exec_shell, sizeof( console_exec_shell ) - 1, LIB_SYS_STREAM_FLOW_out_to_parent_in )) ) return 0;
                }

		// check for events from interface
		if( ! console_the_master_of_puppets ) lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &console_interface );
		else {
			// get a key from the keyboard buffer
			uint16_t key = lib_sys_keyboard();

			// prepare the message
			struct STD_IPC_STRUCTURE_KEYBOARD message;
			message.type = LIB_SYS_IPC_TYPE_KEYBOARD;

			// and the key itself
			message.key = key;

			// send to child
			lib_sys_ipc_send( console_pid_child, (uint8_t *) &message );
		}

		// receive pending messages
		if( lib_sys_ipc_receive( (struct STD_IPC_STRUCTURE *) &console_message, LIB_SYS_IPC_TYPE_KEYBOARD ) )
			// send to process
			lib_sys_ipc_send( console_pid_child, (uint8_t *) &console_message.data );

		// get data from input stream
		uint64_t console_stream_length = lib_sys_stream_in( console_stream_in );

		// if there is stream content
		if( console_stream_length ) {
			// disable cursor, no CPU power waste
			lib_terminal_cursor_disable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// parse all characters from stream
			for( uint32_t i = 0; i < console_stream_length; ) {
				// start sequence and minimum number of characters?
				if( console_stream_in[ i ] == STATIC_ASCII_CARET && console_stream_in[ i + 1 ] == STATIC_ASCII_BRACKET_SQUARE_OPEN ) {
					// modify window header?
					if( console_stream_in[ i + 2 ] == STATIC_ASCII_LETTER_h ) {
						// number of characters for name of window
						uint8_t n = 0;
						while( console_stream_in[ i + 3 + n ] != STATIC_ASCII_BRACKET_SQUARE_CLOSE ) n++;

						if( ! console_the_master_of_puppets ) {
							// update inteface properties
							for( uint8_t m = 0; m < n; m++ ) console_interface.name[ m ] = console_stream_in[ i + 3 + m ]; console_interface.length = n;

							// rename the window
							lib_interface_name( (struct LIB_INTERFACE_STRUCTURE *) &console_interface );

							// define properties of requesting window
							struct WM_STRUCTURE_IPC_WINDOW_rename *rename = (struct WM_STRUCTURE_IPC_WINDOW_rename *) console_message.data;
							rename -> ipc.type = LIB_SYS_IPC_TYPE_WINDOW_MANAGER;
							rename -> ipc.function = WM_IPC_FUNCTION_rename;
							rename -> id = console_interface.id;
							rename -> length = n; if( n > WM_OBJECT_NAME_length ) rename -> length = WM_OBJECT_NAME_length;
							for( uint16_t n = 0; n < rename -> length; n++ ) rename -> name[ n ] = console_interface.name[ n ];

							// request window rename
							lib_sys_ipc_send( console_framebuffer.pid, (uint8_t *) rename );
						}

						// sequence processed
						i += STATIC_SEQUENCE_LENGTH + n;

						// sequence processed
						continue;
					}

					// color change?
					if( console_stream_in[ i + 2 ] == STATIC_ASCII_LETTER_c ) {
						// background?
						if( console_stream_in[ i + 3 ] != STATIC_ASCII_ASTERISK ) {
							// get the color number
							uint8_t c = console_stream_in[ i + 3 ] - STATIC_ASCII_DIGIT_0;
							if( c > 9 ) c -= STATIC_ASCII_DIFFERENCE_0_A;

							// set the color based on the index from the color table
							console_terminal.color_background = console_colors[ c ];
						}

						// foreground?
						if( console_stream_in[ i + 4 ] != STATIC_ASCII_ASTERISK ) {
							// get the color number
							uint8_t c = console_stream_in[ i + 4 ] - STATIC_ASCII_DIGIT_0;
							if( c > 9 ) c -= STATIC_ASCII_DIFFERENCE_0_A;

							// set the color based on the index from the color table
							console_terminal.color_foreground = console_colors[ c ];
						}

						// sequence processed
						i += STATIC_SEQUENCE_LENGTH + 1;

						// sequence processed
						continue;
					}

					// terminal operation?
					if( console_stream_in[ i + 2 ] == STATIC_ASCII_LETTER_t ) {
						// clear functions?
						if( console_stream_in[ i + 3 ] == STATIC_ASCII_DIGIT_0 ) {
							// operation
							switch( console_stream_in[ i + 4 ] ) {
								case STATIC_ASCII_DIGIT_0: {
									// clear terminal space
									lib_terminal_drain( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// done
									break;
								}
								case STATIC_ASCII_DIGIT_1: {
									// clear current line
									lib_terminal_drain_line( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// done
									break;
								}
								case STATIC_ASCII_DIGIT_2: {
									// enable clean character
									console_terminal.flags |= LIBRARY_TERMINAL_FLAG_clean;

									// done
									break;
								}
								case STATIC_ASCII_DIGIT_3: {
									// enable clean character
									console_terminal.flags &= ~LIBRARY_TERMINAL_FLAG_clean;

									// done
									break;
								}
							}

							// sequence processed
							i += STATIC_SEQUENCE_LENGTH + 1;

							// sequence processed
							continue;
						}

						// cursor position manipulation
						if( console_stream_in[ i + 3 ] == STATIC_ASCII_DIGIT_2 ) {
							// operation
							switch( console_stream_in[ i + 4 ] ) {
								case STATIC_ASCII_DIGIT_0: {
									// restore cursor position
									console_terminal.cursor_x = EMPTY;
									console_terminal.cursor_y = EMPTY;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_1: {
									// enable cursor
									lib_terminal_cursor_enable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_2: {
									// disable cursor
									lib_terminal_cursor_disable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_3: {
									// save cursor position
									console_cursor_x = console_terminal.cursor_x;
									console_cursor_y = console_terminal.cursor_y;

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_4: {
									// restore cursor position
									console_terminal.cursor_x = console_cursor_x;
									console_terminal.cursor_y = console_cursor_y;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_5: {
									// move cursor up
									if( console_terminal.cursor_y ) console_terminal.cursor_y--;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_6: {
									// move cursor up
									console_terminal.cursor_y++;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_7: {
									// move cursor up
									if( console_terminal.cursor_x ) console_terminal.cursor_x--;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
								case STATIC_ASCII_DIGIT_8: {
									// move cursor up
									console_terminal.cursor_x++;

									// update cursor properties
									lib_terminal_cursor_set( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

									// ready
									break;
								}
							}

							// sequence processed
							i += STATIC_SEQUENCE_LENGTH + 1;

							// sequence processed
							continue;
						}
					}
				}

				// display the character
				lib_terminal_char( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal, console_stream_in[ i ] );
				i++;
			}

			// turn on the cursor
			lib_terminal_cursor_enable( (struct LIB_TERMINAL_STRUCTURE *) &console_terminal );

			// we are the captain?
			if( console_the_master_of_puppets ) {
				// copy the contents of the buffer to the memory space of the graphics card
				for( uint32_t pixel = 0; pixel < (console_framebuffer.width_pixel * console_framebuffer.height_pixel); pixel++ )
					console_framebuffer.base_address[ pixel ] = console_cache[ pixel ];
			} else
				// update window content on screen
				console_interface.window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_flush;
		} else {
			// set new meta properties
			console_stream_meta.x = console_terminal.cursor_x;
			console_stream_meta.y = console_terminal.cursor_y;

			// update metadata of input stream
			lib_sys_stream_set( (uint8_t *) &console_stream_meta, LIB_SYS_STREAM_in );
		}
	}

	// end of execution
	return 0;
}
