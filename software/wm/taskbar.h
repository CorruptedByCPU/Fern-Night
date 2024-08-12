/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	WM_TASKBAR
	#define	WM_TASKBAR

	#define	WM_TASKBAR_BACKGROUND_COLOR_active	0xE0202020
	#define	WM_TASKBAR_BACKGROUND_COLOR_visible	0xE0181818
	#define	WM_TASKBAR_BACKGROUND_COLOR_invisible	STATIC_COLOR_black
	#define	WM_TASKBAR_BACKGROUND_COLOR_default	0xE0080808

	void wm_thread_taskbar();
	void wm_thread_taskbar_init();
	void wm_thread_taskbar_windows();
	void wm_thread_taskbar_list();
	void wm_thread_taskbar_clock( uint64_t time );
#endif