/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	#include	"../kernel/config.h"
	#include	"../kernel/driver/ps2.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/color.h"
	#include	"../library/shared/font.h"
	#include	"../library/shared/image.h"
	#include	"../library/shared/integer.h"
	#include	"../library/shared/interface.c"
	#include	"../library/shared/json.h"
	#include	"../library/shared/sys.h"
	// window manager ------------------------------------------------------
	#include	"wm.h"
	#include	"wm/config.h"
	#include	"wm/clock.h"
	#include	"wm/event.h"
	#include	"wm/fill.h"
	#include	"wm/init.h"
	#include	"wm/ipc.h"
	#include	"wm/menu.h"
	#include	"wm/object.h"
	#include	"wm/sync.h"
	#include	"wm/taskbar.h"
	#include	"wm/zone.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, functions, routines
	//----------------------------------------------------------------------
	// window manager ------------------------------------------------------
	#include	"wm/data.c"
	#include	"wm/event.c"
	#include	"wm/fill.c"
	#include	"wm/init.c"
	#include	"wm/ipc.c"
	#include	"wm/menu.c"
	#include	"wm/object.c"
	#include	"wm/sync.c"
	#include	"wm/taskbar.c"
	#include	"wm/zone.c"
	//======================================================================

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	wm_init();

	while( TRUE ) {
		// check upcomming event from mouse/keyboard
		wm_event();

		// check which objects have been recently updated
		wm_object();

		// process modified zones of objects
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// synchronize buffer with video memory space
		wm_sync();
	}

	// end of execution
	return 0;
}
