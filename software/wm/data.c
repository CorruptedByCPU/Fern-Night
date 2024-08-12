/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct LIB_SYS_STRUCTURE_FRAMEBUFFER wm_framebuffer = { EMPTY };
struct LIB_SYS_STRUCTURE_MOUSE wm_mouse = { EMPTY };
struct LIB_SYS_STRUCTURE_STORAGE file_cursor = { EMPTY, EMPTY, 10, "cursor.tga" };
struct LIB_SYS_STRUCTURE_STORAGE file_wallpaper = { EMPTY, EMPTY, 13, "wallpaper.tga" };

// uint8_t debug_interface[] = "...";
MACRO_IMPORT_FILE_AS_STRING( clock_json, "software/wm/clock.json" );

// uint8_t menu_json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( menu_json, "software/wm/menu.json" );

struct LIB_INTERFACE_STRUCTURE menu_interface = { (uintptr_t) &menu_json, 4, "Menu" };

uint64_t wm_pid_number = EMPTY;

uint8_t wm_exec_console[] = "console";

uintptr_t *wm_list_base_address = EMPTY;
uint64_t wm_list_count = EMPTY;

uint8_t wm_taskbar_menu_semaphore = FALSE;

uintptr_t *wm_taskbar_elements = EMPTY;

struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;
struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
uint64_t wm_object_count = EMPTY;
uint8_t wm_object_id = EMPTY;
uint8_t	wm_object_semaphore = FALSE;

struct WM_STRUCTURE_OBJECT wm_object_framebuffer = { EMPTY };
struct WM_STRUCTURE_OBJECT *wm_object_cursor;
struct WM_STRUCTURE_OBJECT *wm_object_workbench;
struct WM_STRUCTURE_OBJECT *wm_object_taskbar;
struct WM_STRUCTURE_OBJECT *wm_object_menu;

volatile uint8_t wm_taskbar_semaphore = FALSE;

uint8_t	wm_object_selected_semaphore = FALSE;

uint8_t	wm_mouse_button_left_semaphore;
uint8_t	wm_mouse_button_right_semaphore;
struct WM_STRUCTURE_OBJECT *wm_object_selected = EMPTY;

struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;
uint64_t wm_zone_count = EMPTY;

struct LIB_SYS_STRUCTURE_IPC wm_message = { EMPTY };

uint16_t wm_keyboard_status_alt_left = EMPTY;

uint8_t wm_object_name_default[] = "[unnamed]";
