/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_RGL
	#define	LIB_RGL

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"sys.h"
	//======================================================================

	struct	LIB_RGL_STRUCTURE {
		uint16_t	width;
		uint16_t	height;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	scanline;
		uint32_t	color_background;
		uint32_t	*workbench_address;
	};

	struct	LIB_RGL_STRUCTURE_SQUARE {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		uint32_t	color;
	};

	void lib_rgl( struct LIB_RGL_STRUCTURE *rgl );
	void lib_rgl_circle( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint64_t radius, float precision, uint32_t color );
	void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl );
	void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl );
	void lib_rgl_square( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint16_t width, uint16_t height, uint32_t color );
#endif