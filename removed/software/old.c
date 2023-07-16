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
	#include	"../library/shared/font.h"
	#include	"../library/shared/integer.h"
	#include	"../library/shared/interface.c"
	#include	"../library/shared/random.h"
	#include	"../library/shared/sys.c"
	#include	"../library/shared/string.h"
	#include	"../library/shared/math.c"
	// external ------------------------------------------------------------
	#include	"wm.h"
	//----------------------------------------------------------------------
	// variables, functions, routines
	//----------------------------------------------------------------------
	// #include	"taris/config.h"
	// #include	"taris/data.c"
	// #include	"taris/init.c"
	//======================================================================

#define	LIB_RGL_LAYER_TYPE_null		0
#define	LIB_RGL_LAYER_TYPE_circle	1
#define	LIB_RGL_LAYER_TYPE_line		2
#define	LIB_RGL_LAYER_TYPE_triangle	3
#define	LIB_RGL_LAYER_TYPE_square	4
#define	LIB_RGL_LAYER_TYPE_point	5

#define	LIB_RGL_LAYER_FLAG_filled	1

typedef	struct VECTOR2F {
	double	x;
	double	y;
} vector2f;

typedef	struct VECTOR2D {
	int64_t	x;
	int64_t	y;
} vector2d;

	struct	LIB_RGL_STRUCTURE_LAYER_POINT {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 1 ];
		vector2f	save[ 1 ];
	};

	struct	LIB_RGL_STRUCTURE_LAYER_CIRCLE {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 1 ];
		double		radius;
		vector2f	save[ 1 ];
		double		save_radius;
	};

	struct	LIB_RGL_STRUCTURE_LAYER_LINE {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 2 ];
		vector2f	save[ 2 ];
	};

	struct	LIB_RGL_STRUCTURE_LAYER_TRIANGLE {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 3 ];
		vector2f	save[ 3 ];
	};

	struct	LIB_RGL_STRUCTURE_LAYER_SQUARE {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 4 ];
		vector2f	save[ 4 ];
	};

	struct	LIB_RGL_STRUCTURE_LAYER_VIRTUAL {
		uint8_t		flags;
		uint32_t	color;
		vector2f	point[ 4 ];
	};

	struct	LIB_RGL_STRUCTURE_LAYER {
		uint8_t		type;
		void		*layer;
	};

	struct	LIB_RGL_STRUCTURE {
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	scanline_pixel;
		uint32_t	color_background;
		uint32_t	*workbench_address;
		struct LIB_RGL_STRUCTURE_LAYER	*layers;
	};

// uint8_t taris_json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( json, "software/old.json" );

struct LIB_INTERFACE_STRUCTURE interface = { (uintptr_t) &json };

struct LIB_RGL_STRUCTURE rgl = { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 0xFF000000, EMPTY };

uint64_t lib_rgl_layer_limit = STATIC_PAGE_SIZE_byte / sizeof( struct LIB_RGL_STRUCTURE_LAYER );

void close( void ) {
	// end of execution
	lib_sys_exit();
}

void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl ) {
	// clean up workbench space
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> workbench_address[ (y * rgl -> width_pixel) + x ] = rgl -> color_background;
}

void lib_rgl( struct LIB_RGL_STRUCTURE *rgl ) {
	// calculate size of base area in Bytes
	rgl -> size_byte = (rgl -> width_pixel * rgl -> height_pixel) << STATIC_VIDEO_DEPTH_shift;

	// prepare workbench space
	rgl -> workbench_address = (uint32_t *) malloc( rgl -> size_byte );

	// create list of layers
	rgl -> layers = (void *) malloc( lib_rgl_layer_limit * sizeof( struct LIB_RGL_STRUCTURE_LAYER ) );
}

uint64_t lib_rgl_add_line( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_LAYER_LINE *line ) {
	// check for free entry
	uint64_t l = 1; for( ; l < lib_rgl_layer_limit; l++ ) if( rgl -> layers[ l ].type == LIB_RGL_LAYER_TYPE_null ) break;

	// entry not available?
	if( l == lib_rgl_layer_limit ) return EMPTY;	// yes

	// set new layer
	rgl -> layers[ l ].type = LIB_RGL_LAYER_TYPE_line;
	rgl -> layers[ l ].layer = malloc( sizeof( struct LIB_RGL_STRUCTURE_LAYER_LINE ) );

	// update properties of layer
	volatile struct LIB_RGL_STRUCTURE_LAYER_LINE *layer = (struct LIB_RGL_STRUCTURE_LAYER_LINE *) rgl -> layers[ l ].layer;
	layer -> point[ 0 ] = line -> point[ 0 ];
	layer -> point[ 1 ] = line -> point[ 1 ];
	layer -> color = line -> color;
	layer -> save[ 0 ] = line -> point[ 0 ];
	layer -> save[ 1 ] = line -> point[ 1 ];

	// return ID of layer
	return l;
}

uint64_t lib_rgl_add_circle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *circle ) {
	// check for free entry
	uint64_t l = 1; for( ; l < lib_rgl_layer_limit; l++ ) if( rgl -> layers[ l ].type == LIB_RGL_LAYER_TYPE_null ) break;

	// entry not available?
	if( l == lib_rgl_layer_limit ) return EMPTY;	// yes

	// set new layer
	rgl -> layers[ l ].type = LIB_RGL_LAYER_TYPE_circle;
	rgl -> layers[ l ].layer = malloc( sizeof( struct LIB_RGL_STRUCTURE_LAYER_CIRCLE ) );

	// update properties of layer
	struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *layer = (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) rgl -> layers[ l ].layer;
	layer -> flags = circle -> flags;
	layer -> point[ 0 ] = circle -> point[ 0 ];
	layer -> radius = circle -> radius;
	layer -> color = circle -> color;
	layer -> save[ 0 ] = circle -> point[ 0 ];
	layer -> save_radius = circle -> radius;

	// return ID of layer
	return l;
}

uint64_t lib_rgl_add_square( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_LAYER_SQUARE *square ) {
	// check for free entry
	uint64_t l = 1; for( ; l < lib_rgl_layer_limit; l++ ) if( rgl -> layers[ l ].type == LIB_RGL_LAYER_TYPE_null ) break;

	// entry not available?
	if( l == lib_rgl_layer_limit ) return EMPTY;	// yes

	// set new layer
	rgl -> layers[ l ].type = LIB_RGL_LAYER_TYPE_square;
	rgl -> layers[ l ].layer = malloc( sizeof( struct LIB_RGL_STRUCTURE_LAYER_SQUARE ) );

	// update properties of layer
	struct LIB_RGL_STRUCTURE_LAYER_SQUARE *layer = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) rgl -> layers[ l ].layer;
	layer -> flags = square -> flags;
	layer -> point[ 0 ] = square -> point[ 0 ];
	layer -> point[ 1 ] = square -> point[ 1 ];
	layer -> point[ 2 ] = square -> point[ 2 ];
	layer -> point[ 3 ] = square -> point[ 3 ];
	layer -> color = square -> color;
	layer -> save[ 0 ] = square -> point[ 0 ];
	layer -> save[ 1 ] = square -> point[ 1 ];
	layer -> save[ 2 ] = square -> point[ 2 ];
	layer -> save[ 3 ] = square -> point[ 3 ];

	// return ID of layer
	return l;
}

uint64_t lib_rgl_add_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *triangle ) {
	// check for free entry
	uint64_t l = 1; for( ; l < lib_rgl_layer_limit; l++ ) if( rgl -> layers[ l ].type == LIB_RGL_LAYER_TYPE_null ) break;

	// entry not available?
	if( l == lib_rgl_layer_limit ) return EMPTY;	// yes

	// set new layer
	rgl -> layers[ l ].type = LIB_RGL_LAYER_TYPE_triangle;
	rgl -> layers[ l ].layer = malloc( sizeof( struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE ) );

	// update properties of layer
	struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *layer = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) rgl -> layers[ l ].layer;
	layer -> flags = triangle -> flags;
	layer -> point[ 0 ] = triangle -> point[ 0 ];
	layer -> point[ 1 ] = triangle -> point[ 1 ];
	layer -> point[ 2 ] = triangle -> point[ 2 ];
	layer -> color = triangle -> color;
	layer -> save[ 0 ] = triangle -> point[ 0 ];
	layer -> save[ 1 ] = triangle -> point[ 1 ];
	layer -> save[ 2 ] = triangle -> point[ 2 ];

	// return ID of layer
	return l;
}

void lib_rgl_rotate( struct LIB_RGL_STRUCTURE *rgl, uint64_t id, int64_t angle ) {
	// keep in range
	angle = angle % 360;
	if (angle > 180) angle -= 360;

	// convert to radians
	float radians = angle * LIB_MATH_PI / 180;

	// calculate angle transformation
	float sinus = lib_math_sin( radians );
	float cosinus = lib_math_cos( radians );

	// local variables
	float x, y;

	// properties of layer
	switch( rgl -> layers[ id ].type ) {
		case LIB_RGL_LAYER_TYPE_line: {
			// line properties
			struct LIB_RGL_STRUCTURE_LAYER_LINE *line = (struct LIB_RGL_STRUCTURE_LAYER_LINE *) rgl -> layers[ id ].layer;

			// rotate

			x = (cosinus * line -> point[ 0 ].x) + (-sinus * line -> point[ 0 ].y);
			y = (sinus * line -> point[ 0 ].x) + (cosinus * line -> point[ 0 ].y);
			line -> point[ 0 ].x = x;
			line -> point[ 0 ].y = y;

			x = (cosinus * line -> point[ 1 ].x) + (-sinus * line -> point[ 1 ].y);
			y = (sinus * line -> point[ 1 ].x) + (cosinus * line -> point[ 1 ].y);
			line -> point[ 1 ].x = x;
			line -> point[ 1 ].y = y;

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_square: {
			// line properties
			struct LIB_RGL_STRUCTURE_LAYER_SQUARE *square = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) rgl -> layers[ id ].layer;

			// rotate

			x = (cosinus * square -> point[ 0 ].x) + (-sinus * square -> point[ 0 ].y);
			y = (sinus * square -> point[ 0 ].x) + (cosinus * square -> point[ 0 ].y);
			square -> point[ 0 ].x = x;
			square -> point[ 0 ].y = y;

			x = (cosinus * square -> point[ 1 ].x) + (-sinus * square -> point[ 1 ].y);
			y = (sinus * square -> point[ 1 ].x) + (cosinus * square -> point[ 1 ].y);
			square -> point[ 1 ].x = x;
			square -> point[ 1 ].y = y;

			x = (cosinus * square -> point[ 2 ].x) + (-sinus * square -> point[ 2 ].y);
			y = (sinus * square -> point[ 2 ].x) + (cosinus * square -> point[ 2 ].y);
			square -> point[ 2 ].x = x;
			square -> point[ 2 ].y = y;

			x = (cosinus * square -> point[ 3 ].x) + (-sinus * square -> point[ 3 ].y);
			y = (sinus * square -> point[ 3 ].x) + (cosinus * square -> point[ 3 ].y);
			square -> point[ 3 ].x = x;
			square -> point[ 3 ].y = y;

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_triangle: {
			// line properties
			struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *triangle = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) rgl -> layers[ id ].layer;

			// rotate

			x = (cosinus * triangle -> point[ 0 ].x) + (-sinus * triangle -> point[ 0 ].y);
			y = (sinus * triangle -> point[ 0 ].x) + (cosinus * triangle -> point[ 0 ].y);
			triangle -> point[ 0 ].x = x;
			triangle -> point[ 0 ].y = y;

			x = (cosinus * triangle -> point[ 1 ].x) + (-sinus * triangle -> point[ 1 ].y);
			y = (sinus * triangle -> point[ 1 ].x) + (cosinus * triangle -> point[ 1 ].y);
			triangle -> point[ 1 ].x = x;
			triangle -> point[ 1 ].y = y;

			x = (cosinus * triangle -> point[ 2 ].x) + (-sinus * triangle -> point[ 2 ].y);
			y = (sinus * triangle -> point[ 2 ].x) + (cosinus * triangle -> point[ 2 ].y);
			triangle -> point[ 2 ].x = x;
			triangle -> point[ 2 ].y = y;

			// parsed
			break;
		}
	}
}

void lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color ) {
	int64_t dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int64_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int64_t err = dx + dy, e2;

	for( ; ; ) {
		rgl -> workbench_address[ (y0 * rgl -> width_pixel) + x0 ] = color;

		if (x0 == x1 && y0 == y1) break;

		e2 = 2 * err;
	
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void lib_rgl_circle( struct LIB_RGL_STRUCTURE *rgl, int64_t x, int64_t y, uint64_t radius, float precision, uint32_t color ) {
	// for every angle of circle
	for( float a = 0.0f; a < LIB_MATH_PI_M_2; a += precision )
		// draw a pixel
		rgl -> workbench_address[ ((uint64_t) (radius * lib_math_sin( a ) + y) * rgl -> width_pixel) + (uint64_t) (radius * lib_math_cos( a ) + x) ] = color;
}

void lib_rgl_save( struct LIB_RGL_STRUCTURE *rgl, uint64_t id ) {
	// entry properties
	switch( rgl -> layers[ id ].type ) {
		case LIB_RGL_LAYER_TYPE_line: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_LINE *line = (struct LIB_RGL_STRUCTURE_LAYER_LINE *) rgl -> layers[ id ].layer;

			// store current points
			line -> save[ 0 ] = line -> point[ 0 ];
			line -> save[ 1 ] = line -> point[ 1 ];

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_circle: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *circle = (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) rgl -> layers[ id ].layer;

			// store current points
			circle -> save[ 0 ] = circle -> point[ 0 ];
			circle -> save_radius = circle -> radius;

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_square: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_SQUARE *square = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) rgl -> layers[ id ].layer;

			// store current points
			square -> save[ 0 ] = square -> point[ 0 ];
			square -> save[ 1 ] = square -> point[ 1 ];
			square -> save[ 2 ] = square -> point[ 2 ];
			square -> save[ 3 ] = square -> point[ 3 ];

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_triangle: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *triangle = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) rgl -> layers[ id ].layer;

			// store current points
			triangle -> save[ 0 ] = triangle -> point[ 0 ];
			triangle -> save[ 1 ] = triangle -> point[ 1 ];
			triangle -> save[ 2 ] = triangle -> point[ 2 ];

			// parsed
			break;
		}
	}
}

void lib_rgl_restore( struct LIB_RGL_STRUCTURE *rgl, uint64_t id ) {
	// entry ptoperties
	switch( rgl -> layers[ id ].type ) {
		case LIB_RGL_LAYER_TYPE_line: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_LINE *line = (struct LIB_RGL_STRUCTURE_LAYER_LINE *) rgl -> layers[ id ].layer;

			// restore previous points
			line -> point[ 0 ] = line -> save[ 0 ];
			line -> point[ 1 ] = line -> save[ 1 ];

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_circle: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *circle = (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) rgl -> layers[ id ].layer;

			// restore previous points
			circle -> point[ 0 ] = circle -> save[ 0 ];
			circle -> radius = circle -> save_radius;

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_square: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_SQUARE *square = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) rgl -> layers[ id ].layer;

			// restore previous points
			square -> point[ 0 ] = square -> save[ 0 ];
			square -> point[ 1 ] = square -> save[ 1 ];
			square -> point[ 2 ] = square -> save[ 2 ];
			square -> point[ 3 ] = square -> save[ 3 ];

			// parsed
			break;
		}

		case LIB_RGL_LAYER_TYPE_triangle: {
			// layer properties
			struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *triangle = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) rgl -> layers[ id ].layer;

			// restore previous points
			triangle -> point[ 0 ] = triangle -> save[ 0 ];
			triangle -> point[ 1 ] = triangle -> save[ 1 ];
			triangle -> point[ 2 ] = triangle -> save[ 2 ];

			// parsed
			break;
		}
	}
}

void lib_rgl_remove( struct LIB_RGL_STRUCTURE *rgl, uint64_t id ) {
	// release layer
	free( rgl -> layers[ id ].layer );

	// remove it from list
	rgl -> layers[ id ].type = EMPTY;
}

void lib_rgl_transform( struct LIB_RGL_STRUCTURE *rgl, uint64_t id, int64_t x, int64_t y ) {
		// virtual layer properties
		struct LIB_RGL_STRUCTURE_LAYER_VIRTUAL *virtual = (struct LIB_RGL_STRUCTURE_LAYER_VIRTUAL *) rgl -> layers[ id ].layer;

		// by layer type
		switch( rgl -> layers[ id ].type ) {
			case LIB_RGL_LAYER_TYPE_line: {
				// move all points
				virtual -> point[ 0 ].x += x;
				virtual -> point[ 0 ].y += y;
				virtual -> point[ 1 ].x += x;
				virtual -> point[ 1 ].y += y;

				// parsed
				break;
			}

			case LIB_RGL_LAYER_TYPE_square: {
				// move all points
				virtual -> point[ 0 ].x += x;
				virtual -> point[ 0 ].y += y;
				virtual -> point[ 1 ].x += x;
				virtual -> point[ 1 ].y += y;
				virtual -> point[ 2 ].x += x;
				virtual -> point[ 2 ].y += y;
				virtual -> point[ 3 ].x += x;
				virtual -> point[ 3 ].y += y;

				// parsed
				break;
			}
			
			case LIB_RGL_LAYER_TYPE_circle: {
				// move all points
				virtual -> point[ 0 ].x += x;
				virtual -> point[ 0 ].y += y;

				// parsed
				break;
			}

			case LIB_RGL_LAYER_TYPE_triangle: {
				// move all points
				virtual -> point[ 0 ].x += x;
				virtual -> point[ 0 ].y += y;
				virtual -> point[ 1 ].x += x;
				virtual -> point[ 1 ].y += y;
				virtual -> point[ 2 ].x += x;
				virtual -> point[ 2 ].y += y;

				// parsed
				break;
			}
		}
}

void static lib_rgl_triangle_flat_top( struct LIB_RGL_STRUCTURE *rgl, vector2f v1, vector2f v2, vector2f v3, uint32_t color ) {
	float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
	float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

	float curx1 = v3.x;
	float curx2 = v3.x;

	for( int64_t scanlineY = v3.y; scanlineY > v1.y; scanlineY-- ) {
		lib_rgl_line( rgl, (int64_t) curx1, (int64_t) scanlineY, (int64_t) curx2, (int64_t) scanlineY, color );

		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

void static lib_rgl_triangle_flat_bottom( struct LIB_RGL_STRUCTURE *rgl, vector2f v1, vector2f v2, vector2f v3, uint32_t color ) {
	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

	float curx1 = v1.x;
	float curx2 = v1.x;

	for( int64_t scanlineY = v1.y + 0.1f; scanlineY < v2.y; scanlineY++ ) {
		lib_rgl_line( rgl, (int64_t) curx1, (int64_t) scanlineY, (int64_t) curx2, (int64_t) scanlineY, color );

		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, vector2d p0, vector2d p1, vector2d p2, uint32_t color ) {
	int64_t dx =  abs((int64_t) p2.x - (int64_t) p1.x), sx = (int64_t) p1.x < (int64_t) p2.x ? 1 : -1;
	int64_t dy = -abs((int64_t) p2.y - (int64_t) p1.y), sy = (int64_t) p1.y < (int64_t) p2.y ? 1 : -1; 
	int64_t err = dx + dy, e2;

	// draw triangle on screen
	for( ; ; ) {
		// line by line
		lib_rgl_line( rgl, p1.x, p1.y, p0.x, p0.y, color );

		// triangle ready?
		if (p1.x == p2.x && p1.y == p2.y) break;	// yes

		e2 = 2 * err;
	
		if (e2 >= dy) { err += dy; p1.x += sx; }
		if (e2 <= dx) { err += dx; p1.y += sy; }
	}
}

void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl ) {
	// clean up playground
	lib_rgl_clean( rgl );

	// offsets
	uint64_t x = rgl -> width_pixel / 2;
	uint64_t y = rgl -> height_pixel / 2;

	// show every layer
	for( uint64_t i = 0; i < lib_rgl_layer_limit; i++ ) {
		// by layer type
		switch( rgl -> layers[ i ].type ) {
			case LIB_RGL_LAYER_TYPE_line: {
				// line properties
				struct LIB_RGL_STRUCTURE_LAYER_LINE *line = (struct LIB_RGL_STRUCTURE_LAYER_LINE *) rgl -> layers[ i ].layer;

				// draw line on screen
				lib_rgl_line( rgl, line -> point[ 0 ].x + x, (-line -> point[ 0 ].y) + y, line -> point[ 1 ].x + x, (-line -> point[ 1 ].y) + y, line -> color );

				// parsed
				break;
			}

			case LIB_RGL_LAYER_TYPE_square: {
				// square properties
				struct LIB_RGL_STRUCTURE_LAYER_SQUARE *square = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) rgl -> layers[ i ].layer;

				// filled?
				if( square -> flags & LIB_RGL_LAYER_FLAG_filled ) {
					lib_rgl_triangle( rgl, (vector2d) { (int64_t) square -> point[ 0 ].x + x, (int64_t) -square -> point[ 0 ].y + y }, (vector2d) { (int64_t) square -> point[ 1 ].x + x, (int64_t) -square -> point[ 1 ].y + y }, (vector2d) { (int64_t) square -> point[ 2 ].x + x, (int64_t) -square -> point[ 2 ].y + y }, square -> color );
					lib_rgl_triangle( rgl, (vector2d) { (int64_t) square -> point[ 0 ].x + x, (int64_t) -square -> point[ 0 ].y + y }, (vector2d) { (int64_t) square -> point[ 2 ].x + x, (int64_t) -square -> point[ 2 ].y + y }, (vector2d) { (int64_t) square -> point[ 3 ].x + x, (int64_t) -square -> point[ 3 ].y + y }, square -> color );
				} else {
					// draw square on screen
					lib_rgl_line( rgl, square -> point[ 0 ].x + x, -square -> point[ 0 ].y + y, square -> point[ 1 ].x + x, -square -> point[ 1 ].y + y, square -> color );
					lib_rgl_line( rgl, square -> point[ 1 ].x + x, -square -> point[ 1 ].y + y, square -> point[ 2 ].x + x, -square -> point[ 2 ].y + y, square -> color );
					lib_rgl_line( rgl, square -> point[ 2 ].x + x, -square -> point[ 2 ].y + y, square -> point[ 3 ].x + x, -square -> point[ 3 ].y + y, square -> color );
					lib_rgl_line( rgl, square -> point[ 3 ].x + x, -square -> point[ 3 ].y + y, square -> point[ 0 ].x + x, -square -> point[ 0 ].y + y, square -> color );
				}

				// parsed
				break;
			}
			
			case LIB_RGL_LAYER_TYPE_circle: {
				// circle properties
				struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *circle = (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) rgl -> layers[ i ].layer;

				// filled?
				if( circle -> flags & LIB_RGL_LAYER_FLAG_filled )
					// amount of rings
					for( uint64_t r = 0; r < circle -> radius; r++ )
						// for every angle of ring
						for( float a = 0.0f; a < LIB_MATH_PI_M_2; a += 0.01f )
							// draw a pixel
							rgl -> workbench_address[ ((uint64_t) (r * lib_math_sin( a ) + circle -> point[ 0 ].y + y) * rgl -> width_pixel) + (uint64_t) (r * lib_math_cos( a ) + circle -> point[ 0 ].x + x) ] = circle -> color;
				else lib_rgl_circle( rgl, circle -> point[ 0 ].x + x, -circle -> point[ 0 ].y + y, circle -> radius, (float) 0.01f, circle -> color );


				// parsed
				break;
			}

			case LIB_RGL_LAYER_TYPE_triangle: {
				// square properties
				struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *triangle = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) rgl -> layers[ i ].layer;

				// filled?
				if( triangle -> flags & LIB_RGL_LAYER_FLAG_filled ) {
					lib_rgl_triangle( rgl, (vector2d) { (int64_t) triangle -> point[ 0 ].x + x, (int64_t) -triangle -> point[ 0 ].y + y }, (vector2d) { (int64_t) triangle -> point[ 1 ].x + x, (int64_t) -triangle -> point[ 1 ].y + y }, (vector2d) { (int64_t) triangle -> point[ 2 ].x + x, (int64_t) -triangle -> point[ 2 ].y + y }, triangle -> color );
				} else {
					// draw triangle on screen
					lib_rgl_line( rgl, triangle -> point[ 0 ].x + x, -triangle -> point[ 0 ].y + y, triangle -> point[ 1 ].x + x, -triangle -> point[ 1 ].y + y, triangle -> color );
					lib_rgl_line( rgl, triangle -> point[ 1 ].x + x, -triangle -> point[ 1 ].y + y, triangle -> point[ 2 ].x + x, -triangle -> point[ 2 ].y + y, triangle -> color );
					lib_rgl_line( rgl, triangle -> point[ 2 ].x + x, -triangle -> point[ 2 ].y + y, triangle -> point[ 0 ].x + x, -triangle -> point[ 0 ].y + y, triangle -> color );
				}

				// parsed
				break;
			}
		}
	}

	// workbench index
	uint64_t w = EMPTY;

	// synchronize standard output with workbench
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> base_address[ (y * rgl -> scanline_pixel) + x ] = rgl -> workbench_address[ w++ ];
}

void init() {
	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &interface );

	// find control element for close function
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *close_button = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, 1 ); close_button -> event = (void *) close;

	// initialize Raw Graphics Layer
	rgl.width_pixel = interface.width - 2;
	rgl.height_pixel = interface.height - LIB_INTERFACE_HEADER_HEIGHT_pixel - 1;
	rgl.scanline_pixel = interface.width;
	rgl.base_address = interface.base_address + (rgl.scanline_pixel * LIB_INTERFACE_HEADER_HEIGHT_pixel) + 1;
	lib_rgl( (struct LIB_RGL_STRUCTURE *) &rgl );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize gameplay
	init();

	// initial points of circle
	struct LIB_RGL_STRUCTURE_LAYER_CIRCLE circle = { LIB_RGL_LAYER_FLAG_filled, STATIC_COLOR_blue_light, { { 0.0f, 0.0f } }, 48.0f };
	uint64_t circle_id = lib_rgl_add_circle( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) &circle );

	// initial points of square
	struct LIB_RGL_STRUCTURE_LAYER_SQUARE square = { LIB_RGL_LAYER_FLAG_filled, STATIC_COLOR_red_light, { { -34.0f, -34.0f }, { -34.0f, 34.0f }, { 34.0f, 34.0f }, { 34.0f, -34.0f } } };
	uint64_t square_id = lib_rgl_add_square( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) &square );

	// initial points of line
	struct LIB_RGL_STRUCTURE_LAYER_LINE line = { EMPTY, STATIC_COLOR_green_light, { { 0.0f, 0.0f }, { 34.0f, 0.0f } } };
	uint64_t line_id = lib_rgl_add_line( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_LINE *) &line );

	// initial points of triangle
	struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE triangle = { LIB_RGL_LAYER_FLAG_filled, STATIC_COLOR_white, { { 0.0f, 0.0f }, { 16.0f, 16.0f }, { 16.0f, -16.0f } } };
	uint64_t triangle_id = lib_rgl_add_triangle( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) &triangle );
	lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id, 48, 0 );
	lib_rgl_save( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id );

	// initial points of circle
	circle = (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE) { EMPTY, STATIC_COLOR_blue_light, { { 0.0f, 0.0f } }, 48.0f };
	uint64_t circle_id_e = lib_rgl_add_circle( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_CIRCLE *) &circle );

	// initial points of square
	square = (struct LIB_RGL_STRUCTURE_LAYER_SQUARE) { EMPTY, STATIC_COLOR_red_light, { { -34.0f, -34.0f }, { -34.0f, 34.0f }, { 34.0f, 34.0f }, { 34.0f, -34.0f } } };
	uint64_t square_id_e = lib_rgl_add_square( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_SQUARE *) &square );

	// initial points of line
	line = (struct LIB_RGL_STRUCTURE_LAYER_LINE) { EMPTY, STATIC_COLOR_green_light, { { 0.0f, 0.0f }, { 34.0f, 0.0f } } };
	uint64_t line_id_e = lib_rgl_add_line( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_LINE *) &line );

	// initial points of triangle
	triangle = (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE) { EMPTY, STATIC_COLOR_white, { { 0.0f, 0.0f }, { 16.0f, 16.0f }, { 16.0f, -16.0f } } };
	uint64_t triangle_id_e = lib_rgl_add_triangle( (struct LIB_RGL_STRUCTURE *) &rgl, (struct LIB_RGL_STRUCTURE_LAYER_TRIANGLE *) &triangle );
	lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id_e, 48, 0 );
	lib_rgl_save( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id_e );

	// rotate 
	int64_t angle = 0;

	while( TRUE ) {
		// check for events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &interface );

		// next angle
		angle++;

		// rotate by definied angle
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, line_id, -angle );
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, square_id, angle / 2 );
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id, angle );
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, line_id_e, -angle );
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, square_id_e, angle / 2 );
		lib_rgl_rotate( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id_e, angle );

		// move primitives to their location
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, line_id, 64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, circle_id, 64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id, 64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, square_id, 64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, line_id_e, -64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, circle_id_e, -64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id_e, -64, 0 );
		lib_rgl_transform( (struct LIB_RGL_STRUCTURE *) &rgl, square_id_e, -64, 0 );

		// draw inside window
		lib_rgl_flush( (struct LIB_RGL_STRUCTURE *) &rgl );

		// tell window manager to flush window content
		interface.window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_flush;

		// set ogirinal polygons of
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, line_id );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, circle_id );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, square_id );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, line_id_e );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, circle_id_e );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, triangle_id_e );
		lib_rgl_restore( (struct LIB_RGL_STRUCTURE *) &rgl, square_id_e );

		// wait some units
		lib_sys_sleep( 16 );
	}

	// end of execution
	return 0;
}