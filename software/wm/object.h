/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	WM_OBJECT
	#define	WM_OBJECT

	void wm_object();
	void wm_object_activate();
	uintptr_t wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t flags );
	struct WM_STRUCTURE_OBJECT *wm_object_by_id( uint8_t id );
	struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t hidden );
	void wm_object_fragile();
	void wm_object_insert( struct WM_STRUCTURE_OBJECT *object );
	void wm_object_move( struct WM_STRUCTURE_OBJECT *object, int16_t x, int16_t y );
	void wm_object_remove( struct WM_STRUCTURE_OBJECT *object );
	void wm_object_move_up( struct WM_STRUCTURE_OBJECT *selected );
#endif