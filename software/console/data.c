/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

uint64_t console_pid = EMPTY;
uint64_t console_pid_child = -1;

struct LIB_SYS_STRUCTURE_FRAMEBUFFER console_framebuffer = { EMPTY };
uint32_t *console_cache;

struct LIB_SYS_STRUCTURE_IPC console_ipc = { EMPTY };

struct LIB_SYS_STRUCTURE_IPC console_message = { EMPTY };

struct	LIB_TERMINAL_STRUCTURE console_terminal = { CONSOLE_WINDOW_width_pixel - 2, (CONSOLE_WINDOW_height_pixel - LIB_INTERFACE_HEADER_HEIGHT_pixel) - 1, EMPTY, CONSOLE_WINDOW_width_pixel, EMPTY, EMPTY, STATIC_COLOR_gray_light, STATIC_COLOR_black, 0x10, LIBRARY_TERMINAL_FLAG_clean, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY };

// uint8_t console_json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( console_json, "software/console/console.json" );

struct LIB_INTERFACE_STRUCTURE console_interface = { (uintptr_t) &console_json, 7, "Console" };

struct CONSOLE_STRUCTURE_META console_stream_meta = { CONSOLE_WINDOW_width_char, CONSOLE_WINDOW_height_char, EMPTY, EMPTY };

uint8_t console_the_master_of_puppets = FALSE;

uint8_t *console_stream_in;

struct WM_STRUCTURE_IPC_WINDOW_request *data;

uint8_t console_exec_shell[] = "shell";

uint16_t console_cursor_x = EMPTY;
uint16_t console_cursor_y = EMPTY;

uint32_t console_colors[ 16 ] = {
	STATIC_COLOR_black,
	STATIC_COLOR_blue,
	STATIC_COLOR_green,
	STATIC_COLOR_cyan,
	STATIC_COLOR_red,
	STATIC_COLOR_magenta,
	STATIC_COLOR_violet,
	STATIC_COLOR_gray_light,
	STATIC_COLOR_gray,
	STATIC_COLOR_blue_light,
	STATIC_COLOR_green_light,
	STATIC_COLOR_magenta_light,
	STATIC_COLOR_red_light,
	STATIC_COLOR_violet_light,
	STATIC_COLOR_yellow,
	STATIC_COLOR_white
};