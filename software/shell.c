/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// driver --------------------------------------------------------------
	#include	"../kernel/driver/ps2.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/string.h"
	#include	"../library/shared/sys.h"
	// external ------------------------------------------------------------
	#include	"console.h"
	// shell itself --------------------------------------------------------
	#include	"shell/config.h"
	#include	"shell/data.c"
	//======================================================================

uint8_t ls_gradation( uint64_t bytes ) {
	if( bytes >= 0x10000000000 ) return 4;
	else if( bytes >= 0x40000000 ) return 3;
	else if( bytes >= 0x100000 ) return 2;
	else if( bytes >= 0x400 ) return 1;
	return 0;
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// new prompt loop
	while( TRUE ) {
		// set window name
		print( "^[hShell]" );

		// length of string passed by user
		uint16_t shell_command_length = EMPTY;

		// retrieve stream propertries
		struct CONSOLE_STRUCTURE_META meta;
		while( lib_sys_stream_get( (uint8_t *) &meta, LIB_SYS_STREAM_out ) & LIB_SYS_STREAM_FLAG_undefinied );

		// cursor at beginning of line?
		if( meta.x ) print( "\n" );	// no, send new line character

		// show prompt
		print( STATIC_SEQUENCE_COLOR_GREEN"$"STATIC_SEQUENCE_COLOR_DEFAULT" " );

		// main interaction with user
		while( TRUE ) {
			// recieve key
			uint16_t key = getkey();

			// key BACKSPACE?
			if( key == STATIC_ASCII_BACKSPACE ) {
				// if command buffer is empty
				if( ! shell_command_length ) continue;	// ignore key

				// remove previous character from command buffer
				shell_command_length--;

				// send it to stream out
				printf( "%c", key );
			}

			// key RETURN?
			if( key == STATIC_ASCII_RETURN ) {
				// remove orphaned "white" characters from the command buffer
				shell_command_length = lib_string_trim( shell_command, shell_command_length, TRUE );

				// command string isn't empty?
				if( shell_command_length ) {
					// everyone has right to new line
					print( "\n" );

					// extract first word from command string
					uint64_t exec_length = lib_string_word( shell_command, shell_command_length );

					// one of the shell internal commands?
					if( lib_string_length( shell_exec_exit ) == exec_length && lib_string_compare( shell_command, shell_exec_exit, exec_length ) ) return 0;
					if( lib_string_length( shell_exec_clear ) == exec_length && lib_string_compare( shell_command, shell_exec_clear, exec_length ) ) { print( STATIC_SEQUENCE_CLEAR ); break; }

					// by default wait for process end
					uint8_t release = FALSE;

					// if last character of command is an
					if( shell_command[ shell_command_length - 1 ] == STATIC_ASCII_AMPERSAND ) {
						// don't wait for process end
						release = TRUE;

						// remove ampersand from command line
						shell_command_length--;
					}

					// otherwise try to run the program with the given name and parameters
					shell_command[ shell_command_length ] = STATIC_ASCII_TERMINATOR;
					shell_exec_pid = lib_sys_exec( (uint8_t *) &shell_command, shell_command_length + 1, LIB_SYS_STREAM_FLOW_out_to_parent_out );

					// program executed?
					if( ! shell_exec_pid ) {
						// cut arguments
						shell_command[ exec_length ] = EMPTY;

						// show unrecognized command
						printf( "'%s' command not found.", shell_command );

						// show new prompt
						break;
					}
		
					// wait for program end?
					if( release ) break;	// no

					// program finished working?
					while( lib_sys_task_status( shell_exec_pid ) ) {	// no
						// every incoming message
						if( lib_sys_ipc_receive( (struct STD_IPC_STRUCTURE *) &shell_message, LIB_SYS_IPC_TYPE_ANY ) )
							// forward to process
							lib_sys_ipc_send( shell_exec_pid, (uint8_t *) &shell_message.data );
					}

					// enable cursor
					print( STATIC_SEQUENCE_CURSOR_ENABLE );
				}

				// show new prompt
				break;
			}

			// key ALT? (left)
			if( key == DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT ) shell_key_alt_left = TRUE;
			if( key == DRIVER_PS2_KEYBOARD_RELEASE_ALT_LEFT ) shell_key_alt_left = FALSE;

			// key CTRL? (left)
			if( key == DRIVER_PS2_KEYBOARD_PRESS_CTRL_LEFT || key == DRIVER_PS2_KEYBOARD_PRESS_CTRL_RIGHT ) shell_key_ctrl = TRUE;
			if( key == DRIVER_PS2_KEYBOARD_RELEASE_CTRL_LEFT || key == DRIVER_PS2_KEYBOARD_RELEASE_CTRL_RIGHT ) shell_key_ctrl = FALSE;

			// key "c" while CTRL pressed?
			if( key == STATIC_ASCII_LETTER_c && shell_key_ctrl ) {
				// show ellipsis if command cache wasn't empty
				if( shell_command_length ) print( STATIC_SEQUENCE_COLOR_GREEN"." );

				// show new prompt
				break;
			}

			// if there is free space in inside command cache
			if( shell_command_length < SHELL_COMMAND_limit ) {
				// and character in processable range?
				if( ! shell_key_alt_left && (key >= STATIC_ASCII_SPACE && key <= STATIC_ASCII_TILDE) ) {
					// append character to command cache
					shell_command[ shell_command_length++ ] = key;

					// send character to stream out
					printf( "%c", key );
				}
			}
		}
	}

	// end of execution
	return 0;
}