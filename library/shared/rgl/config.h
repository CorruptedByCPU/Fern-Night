 /*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef LIB_RGL
        #define LIB_RGL

	#include	"../../../default.h"

	#define	LIB_RGL_LAYER_LIMIT		(STATIC_PAGE_SIZE_byte / sizeof( struct LIB_RGL_STRUCTURE_LAYER ))

        #define	LIB_RGL_LAYER_TYPE_null		0
        #define	LIB_RGL_LAYER_TYPE_triangle	1

        #define	LIB_RGL_LAYER_FLAG_fill 	1

        typedef	struct VECTOR2F {
                double	x;
                double	y;
        } vector2f;

        typedef	struct VECTOR2D {
                int64_t	x;
                int64_t	y;
        } vector2d;

        typedef	struct VECTOR3F {
                double	x;
                double	y;
		double	z;
		double	w;
        } vector3f;

        typedef	struct VECTOR3D {
                int64_t	x;
                int64_t	y;
		int64_t	z;
        } vector3d;

	struct LIB_RGL_STRUCTURE_MATRIX {
		double	cell[ 4 ][ 4 ];
	};

	struct	LIB_RGL_STRUCTURE {
		uint32_t	color_background;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	scanline_pixel;
		uint32_t	*workbench_address;
		vector3f	camera;
		vector3f        position;
		vector3f	target;
		vector3f	up;
		vector3f	light;
		double		fov;
		uint8_t		wireless;
	};

	struct	LIB_RGL_STRUCTURE_TRIANGLE {
		vector3f	point[ 3 ];
		double		z_depth;
		uint32_t	color;
	};

	struct	LIB_RGL_STRUCTURE_TRIANGLE_2D {
		vector2d	point[ 3 ];
		uint32_t	color;
	};

	struct LIB_RGL_STRUCTURE	*lib_rgl( uint64_t width_pixel, uint64_t height_pixel, uint64_t scanline_pixel, uint32_t *base_address );
	void				lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl );
	void				lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl );
	void				lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color );
	void				lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix );
	uint8_t				lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *parse );
	void				lib_rgl_rotate( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, double x, double y, double z );
	uint32_t			lib_rgl_shade( double lumination );
	void				lib_rgl_sort( struct LIB_RGL_STRUCTURE_TRIANGLE **sorted, uint64_t count );
	void				lib_rgl_translate( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, double x, double y, double z );
	void				lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *triangle );
	double				lib_rgl_vector_product_dot( vector3f *v0, vector3f *v1 );
	void				lib_rgl_vector_normalize( vector3f *v );
	vector3f			lib_rgl_vector_product_cross( vector3f *v0, vector3f *v1 );
	vector3f			lib_rgl_vector_substract( vector3f *from, vector3f *substract );
#endif