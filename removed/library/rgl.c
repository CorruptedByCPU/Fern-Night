/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#include	"math.c"
	#include	"rgl.h"
	//======================================================================

void lib_rgl( struct LIB_RGL_STRUCTURE *rgl ) {
	// calculate size of base area in Bytes
	rgl -> size_byte = (rgl -> width * rgl -> height) << STATIC_VIDEO_DEPTH_shift;

	// prepare workbench space
	rgl -> workbench_address = (uint32_t *) lib_sys_memory_alloc( rgl -> size_byte );

	// clean up 
}

void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl ) {
	// workbench index
	uint64_t w = EMPTY;

	// synchronize standard output with workbench
	for( uint64_t y = 0; y < rgl -> height; y++ )
		for( uint64_t x = 0; x < rgl -> width; x++ )
			rgl -> base_address[ (y * rgl -> scanline) + x ] = rgl -> workbench_address[ w++ ];
}

void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl ) {
	// clean up workbench space
	for( uint64_t y = 0; y < rgl -> height; y++ )
		for( uint64_t x = 0; x < rgl -> width; x++ )
			rgl -> workbench_address[ (y * rgl -> width) + x ] = rgl -> color_background;
}

void lib_rgl_square( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint16_t width, uint16_t height, uint32_t color ) {
	// square inside workbench space?
	if( x >= rgl -> width ) return;		// no
	if( x + width <= EMPTY ) return;	// no
	if( y >= rgl -> height ) return;	// no
	if( y + height <= EMPTY ) return;	// no

	// cut invisible fragments
	if( x < EMPTY ) { width += x; x = EMPTY; }
	if( x + width >= rgl -> width ) width -= (x + width) - rgl -> width;
	if( y < EMPTY ) { height += y; y = EMPTY; }
	if( y + height >= rgl -> height ) height -= (y + height) - rgl -> height;

	// draw square inside workbench space with definied color
	for( int32_t ya = y; ya < y + height; ya++ )
		for( int32_t xa = x; xa < x + width; xa++ )
			rgl -> workbench_address[ (ya * rgl -> width) + xa ] = color;
}

void lib_rgl_circle( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint64_t radius, float precision, uint32_t color ) {
	// for every angle of circle
	for( float a = 0.0f; a < 360.0f; a += precision )
		// draw a pixel
		rgl -> workbench_address[ ((uint64_t) (radius * lib_math_sin( a ) + y) * rgl -> width) + (uint64_t) (radius * lib_math_cos( a ) + x) ] = color;
}