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
	#include	"../library/shared/integer.h"
	#include	"../library/shared/sys.h"
	// external ------------------------------------------------------------
	#include	"console.h"
	//======================================================================

#define	TOP_INTERFACE_HEIGHT_char	6

uint8_t top_gradation( uint64_t bytes ) {
	if( bytes >= 0x10000000000 ) { printf( "%d", bytes / 0x10000000000 ); return 4; }
	else if( bytes >= 1073741824 ) { printf( "%d", bytes / 1073741824 ); return 3; }
	else if( bytes >= 1048576 ) { printf( "%d", bytes / 1048576 ); return 2; }
	else if( bytes >= 1024 ) { printf( "%d", bytes / 1024 ); return 1; }
	else printf( "%d", bytes );

	return 0;
}

uint8_t size_byte[] = "Bytes";
uint8_t size_kib[] = "KiB";
uint8_t size_mib[] = "MiB";
uint8_t size_gib[] = "GiB";
uint8_t size_tib[] = "TiB";

const char *sizes[] = { (const char *) &size_byte, (const char *) &size_kib, (const char *) &size_mib, (const char *) &size_gib, (const char *) &size_tib };

struct LIB_SYS_STRUCTURE_IPC top_message = { EMPTY };

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// set window name and clear screen (color default)
	print( "^[hTask Manager]"STATIC_SEQUENCE_COLOR_DEFAULT"^[t22"STATIC_SEQUENCE_CLEAR );

	uint64_t last_update_task = EMPTY;
	uint64_t last_update_uptime = EMPTY;

	uint16_t selected = 0;

	// retrieve stream propertries
	struct CONSOLE_STRUCTURE_META console_meta;
	while( lib_sys_stream_get( (uint8_t *) &console_meta, LIB_SYS_STREAM_out ) & LIB_SYS_STREAM_FLAG_undefinied );

	// show menu
	print( "^[t20" ); // for( uint16_t i = 0; i < console_meta.height - 1; i++ ) print( "^[t26" );
	print( STATIC_SEQUENCE_COLOR_BACKGROUND_GREEN_LIGHT""STATIC_SEQUENCE_COLOR_BLACK"Q"STATIC_SEQUENCE_COLOR_DEFAULT"uit" );

	while( TRUE ) {
		// reset cursor position
		print( "^[t20"STATIC_SEQUENCE_COLOR_DEFAULT );

		// retrieve current uptime
		volatile uint64_t seconds = lib_sys_uptime() / 1024;	// 1024 ticks per second

		// // time changed?
		// if( last_update_uptime != seconds ) {
		// 	// show header
		// 	print( "^[t20^[t26^[t28^[t28Uptime: " );

		// 	// preserve last update value
		// 	last_update_uptime = seconds;

		// 	// days
		// 	uint64_t d = seconds / 86400;
		// 	if( d ) { printf( STATIC_SEQUENCE_COLOR_GREEN_LIGHT"%d"STATIC_SEQUENCE_COLOR_DEFAULT" day", d ); if( d > 1 ) print( "s" ); print( ", " ); }

		// 	// remove days from seconds
		// 	seconds %= 86400;

		// 	// preserve highlight information
		// 	uint32_t highlight = seconds;

		// 	// hours
		// 	uint8_t h = seconds / 3600;
		// 	print( STATIC_SEQUENCE_COLOR_GRAY );
		// 	if( h < 10 ) print( "0"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	if( ! h ) print( STATIC_SEQUENCE_COLOR_GRAY"0" );
		// 	else printf( STATIC_SEQUENCE_COLOR_GREEN_LIGHT"%d", h );
		// 	if( highlight > 3599 ) print( STATIC_SEQUENCE_COLOR_DEFAULT":"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	else print( STATIC_SEQUENCE_COLOR_GRAY":"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );

		// 	// remove hours from seconds
		// 	seconds %= 3600;

		// 	// minutes
		// 	uint8_t m = seconds / 60;
		// 	if( m < 10 && highlight < 3600 ) print( STATIC_SEQUENCE_COLOR_GRAY"0"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	else if( m < 10 ) print( "0" );
		// 	if( ! m ) printf( STATIC_SEQUENCE_COLOR_GRAY"0"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	else printf( "%d", m );
		// 	if( highlight > 59 ) print( STATIC_SEQUENCE_COLOR_DEFAULT":"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	else print( STATIC_SEQUENCE_COLOR_GRAY":"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );

		// 	// remove minutes from seconds
		// 	seconds %= 60;

		// 	// seconds
		// 	if( seconds < 10 && highlight < 60 ) print( STATIC_SEQUENCE_COLOR_GRAY"0"STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// 	else if( seconds < 10 ) print( "0" );
		// 	printf( "%d", seconds );
		// }

		// retrieve list of tasks
		struct LIB_SYS_STRUCTURE_TASK *task = (struct LIB_SYS_STRUCTURE_TASK *) lib_sys_task();

		// count tasks and find highest process ID number
		uint64_t tasks = EMPTY; uint64_t threads = EMPTY; uint64_t columnWidth = EMPTY;
		while( task[ tasks ].pid && task[ tasks ].name[ 0 ] ) { if( columnWidth < task[ tasks ].pid ) columnWidth = task[ tasks ].pid; tasks++; if( task[ tasks ].flags & LIB_SYS_TASK_FLAG_thread ) threads++; }

		// recieve key
		uint16_t key = getkey();
		if( key == DRIVER_PS2_KEYBOARD_PRESS_ARROW_DOWN ) selected++;
		if( key == DRIVER_PS2_KEYBOARD_PRESS_ARROW_UP ) if( selected ) selected--;
		if( key == STATIC_ASCII_LETTER_q ) {
			for( uint16_t i = 0; i < console_meta.height; i++ ) print( "^[t26" );
			lib_sys_exit();
		}

		// // convert to column width
		// columnWidth = lib_integer_digit_count( columnWidth, STATIC_NUMBER_SYSTEM_decimal );
		// if( columnWidth < 4 ) columnWidth = 4;

		// // show number of tasks currently running
		// print( STATIC_SEQUENCE_COLOR_DEFAULT"^[t20^[t26^[t26^[t28^[t28Tasks: "STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// lib_sys_stream_out_value( tasks, STATIC_NUMBER_SYSTEM_decimal, 1, STATIC_ASCII_SPACE );
		// printf( STATIC_SEQUENCE_COLOR_GRAY", %d thr", threads );

		// // retrieve current memory usage
		// struct LIB_SYS_STRUCTURE_MEMORY memory;
		// lib_sys_memory( (struct LIB_SYS_STRUCTURE_MEMORY *) &memory );

		// // show amount of memory used/availableBartek
		// uint16_t width = (console_meta.width / 2) - 2;
		// print( "^[t20^[t26^[t26" ); for( uint16_t i = 0; i < width; i++ ) print( "^[t28" );
		// print( STATIC_SEQUENCE_COLOR_DEFAULT"["STATIC_SEQUENCE_COLOR_GREEN_LIGHT );
		// uint64_t usage = 100 - (((float) memory.available / memory.total) * 100);
		// usage = ((float) width / 100) * usage;
		// print( STATIC_SEQUENCE_CURSOR_PUSH );
		// print( STATIC_SEQUENCE_COLOR_BACKGROUND_GRAY );
		// for( uint64_t i = 0; i < usage; i++ ) print( " " );
		// print( STATIC_SEQUENCE_COLOR_DEFAULT );
		// for( uint64_t i = 0; i < width - usage; i++ ) print( " " );
		// print( STATIC_SEQUENCE_COLOR_DEFAULT"]" );
		// print( STATIC_SEQUENCE_CURSOR_POP ); print( STATIC_SEQUENCE_COLOR_GRAY_LIGHT ); print( STATIC_SEQUENCE_CLEAR_DISABLE" " );
		// printf( "%d / ", (memory.total - memory.available) << STATIC_MULTIPLE_BY_4_shift );
		// printf( "%d [KiB]", memory.total << STATIC_MULTIPLE_BY_4_shift );
		// print( STATIC_SEQUENCE_CLEAR_ENABLE );
		

		// header of tasks list
		print( "^[t20^[t26^[t26^[t26^[t26"STATIC_SEQUENCE_COLOR_BACKGROUND_GREEN_LIGHT"^[t01"STATIC_SEQUENCE_COLOR_BLACK" PID CPU% Mem. [KiB] Command ('"STATIC_SEQUENCE_COLOR_RED"k"STATIC_SEQUENCE_COLOR_BLACK"'ill me)"STATIC_SEQUENCE_COLOR_DEFAULT"\n" );

		// show each entry
		uint16_t show_only = console_meta.height - TOP_INTERFACE_HEIGHT_char;
		if( selected >= tasks ) selected = tasks - 1;
		for( uint64_t i = 0; i < tasks && show_only; i++ ) {
			// set default color of entry foreground/background
			print( STATIC_SEQUENCE_COLOR_DEFAULT );

			// mark daemon with different color
			if( task[ i ].flags & (LIB_SYS_TASK_FLAG_daemon | LIB_SYS_TASK_FLAG_thread)  ) print( STATIC_SEQUENCE_COLOR_GRAY );
			else print( STATIC_SEQUENCE_COLOR_GRAY_LIGHT );

			// if task selected
			if( i == selected ) print( STATIC_SEQUENCE_COLOR_BACKGROUND_GREEN""STATIC_SEQUENCE_COLOR_WHITE );

			// clean line
			print( STATIC_SEQUENCE_CLEAR_LINE );

			// process ID
			for( uint8_t m = 0; m < columnWidth - lib_integer_digit_count( task[ i ].pid, STATIC_NUMBER_SYSTEM_decimal ); m++ ) print( "^[t28" );
			printf( "%d", task[ i ].pid );

			// memory usage
			for( uint16_t i = 0; i < 11 - 5; i++ ) print( "^[t28" );
			// printf( "%2.1f ", (float) ((float) task[ i ].page / (float) memory.total) * 100.0 );
			printf( "%10d ", task[ i ].page << STATIC_MULTIPLE_BY_4_shift );

			// command line
			printf( "%s\n", task[ i ].name );

			// less to show
			show_only--;
		}

		// // clear rest of lines
		// print( STATIC_SEQUENCE_COLOR_DEFAULT );
		// for( uint64_t i = 0; i < show_only; i++ ) print( STATIC_SEQUENCE_CLEAR_LINE"\n" );

		// release task list
		lib_sys_memory_release( (uintptr_t) task, sizeof( struct LIB_SYS_STRUCTURE_TASK ) * tasks );

		// wait about 128ms for next update
		lib_sys_sleep( 1000 );
	}

	// end of execution
	return 0;
}