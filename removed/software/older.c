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
	#include	"../library/shared/sys.h"
	#include	"../library/shared/math.c"
	#include	"../library/shared/interface.c"
	//======================================================================

        typedef struct VECTOR3F {
                double		x;
                double		y;
		double		z;
		double		w;
        } vector3f;

// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 8, "cube.obj" };
struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 9, "plane.obj" };
// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 10, "teapot.obj" };
// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 8, "axis.obj" };

double speed = 0.02f;

const vector3f forwardC = { 0.0f, 0.0f, 0.01f, 1.0f };
const vector3f rightC = { 0.01f, 0.0f, 0.0f, 1.0f };
const vector3f upC = { 0.0f, 0.01f, 0.0f, 1.0f };

int8_t dx = 0;
int8_t dy = 0;
int8_t dz = 0;
int8_t dv = 0;
int8_t dh = 0;

vector3f move = { 0.0f, 0.0f, 0.0f, 1.0f };

#include	"d/d.c"

int64_t _main( uint64_t argc, uint8_t *argv[] ) { init( argc, argv );
	struct FACE *parsed = (struct FACE *) malloc( sizeof( struct FACE ) * object_faces );
	struct FACE **sorted = (struct FACE **) malloc( sizeof( struct FACE ) * object_faces );

	double h_fov = LIB_MATH_PI / 3.0f;
	double v_fov = h_fov * ((double) interface.width / (double) interface.height);

	mat2D = lib_rgl_return_matrix_view( 45.0f, 0.0f, 1000.0f, interface.width, interface.height );

	while( TRUE ) {
		uint16_t key = getkey();

		if( key == DRIVER_PS2_KEYBOARD_PRESS_ESC ) lib_sys_exit();

		// rotate_x = (double) interface.window_descriptor -> pointer_y_absolute / 10.0f;
		// rotate_y = (double) interface.window_descriptor -> pointer_x_absolute / 10.0f;

struct MATRIX matRX = lib_rgl_return_matrix_rotate_x( -rotate_x );
struct MATRIX matRY = lib_rgl_return_matrix_rotate_y( -rotate_y );
struct MATRIX matRXY = lib_rgl_multiply_matrix( matRX, matRY );

vector3f forwardN = lib_rgl_multiply_vector( forwardC, matRXY );
vector3f rightN = lib_rgl_multiply_vector( rightC, matRXY );
vector3f upN = lib_rgl_multiply_vector( upC, matRXY );

		if( key == 'w' ) dz = 1;	if( key == 'w' + 0x80 || key == 's' + 0x80 ) dz = 0;
		if( key == 's' ) dz = -1;

		if( dz == -1 ) move = lib_rgl_vector_add( move, forwardN );
		if( dz == 1 ) move = lib_rgl_vector_substract( move, forwardN );

		if( key == 'd' ) dx = 1;	if( key == 'a' + 0x80 || key == 'd' + 0x80 ) dx = 0;
		if( key == 'a' ) dx = -1;

		if( dx == -1 ) move = lib_rgl_vector_add( move, rightN );
		if( dx == 1 ) move = lib_rgl_vector_substract( move, rightN );

		if( key == 'r' ) dy = 1;	if( key == 'r' + 0x80 || key == 'f' + 0x80 ) dy = 0;
		if( key == 'f' ) dy = -1;

		if( dy == -1 ) move = lib_rgl_vector_add( move, upN );
		if( dy == 1 ) move = lib_rgl_vector_substract( move, upN );

		for( uint64_t y = 0; y < interface.height; y++ ) for( uint64_t x = 0; x < interface.width; x++ ) workbench[ (y * interface.width) + x ] = 0xFF000000;

		struct MATRIX matW = lib_rgl_return_matrix_world();

		uint64_t sorted_inserted = 0;

		// for all faces in object
		for( uint64_t f = 0; f < object_faces; f++ ) {
			parsed[ f ].p[ 0 ] = lib_rgl_multiply_vector( object[ f ].p[ 0 ], matW );
			parsed[ f ].p[ 1 ] = lib_rgl_multiply_vector( object[ f ].p[ 1 ], matW );
			parsed[ f ].p[ 2 ] = lib_rgl_multiply_vector( object[ f ].p[ 2 ], matW );

			vector3f line1 = lib_rgl_vector_substract( parsed[ f ].p[ 1 ], parsed[ f ].p[ 0 ] );
			vector3f line2 = lib_rgl_vector_substract( parsed[ f ].p[ 2 ], parsed[ f ].p[ 0 ] );

			vector3f normal = lib_rgl_vector_product_cross( line1, line2 );

			normal = lib_rgl_vector_normalize( normal );

			// vector3f ray = lib_rgl_vector_substract( parsed[ f ].p[ 0 ], light );

			// if( lib_rgl_vector_product_dot( normal, ray ) < 0.0f ) {
			if( normal.z < 0.0f ) {
				vector3f light = { 0.0f, 0.0f, -1.0f, 1.0f };
				light = lib_rgl_multiply_vector( light, matRXY );
				lib_rgl_vector_normalize( light );

				double dp = maxf( 0.0f, lib_rgl_vector_product_dot( normal, light ) );

				dp = (15.0f * dp) * 10;
				parsed[ f ].color = ( 0xFF000000 | (uint8_t) dp << 16 | (uint8_t) dp << 8 | (uint8_t) dp );
				// parsed[ f ].color = 0xFFFFFFFF;

				parsed[ f ].p[ 0 ] = lib_rgl_multiply_vector( parsed[ f ].p[ 0 ], mat2D );
				parsed[ f ].p[ 1 ] = lib_rgl_multiply_vector( parsed[ f ].p[ 1 ], mat2D );
				parsed[ f ].p[ 2 ] = lib_rgl_multiply_vector( parsed[ f ].p[ 2 ], mat2D );

				parsed[ f ].z_depth = (parsed[ f ].p[ 0 ].z + parsed[ f ].p[ 1 ].z + parsed[ f ].p[ 2 ].z) / 3.0f;
				sorted[ sorted_inserted++ ] = (struct FACE *) &parsed[ f ];
			}
		}

		lib_rgl_sort_quick( sorted, 1, sorted_inserted - 1 );

		for( uint64_t f = 0; f < sorted_inserted; f++ ) {
			lib_rgl_face( sorted[ f ] );
		}

		lib_rgl_flush();
		interface.window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_flush;

		fps++;
		if( lib_sys_uptime() > sleep ) {
			if( fps_min > fps_counted ) fps_min = fps;
			if( fps_max < fps_counted ) fps_max = fps;

			fps_average += fps;
			fps_counted++;

			printf( "FPS: %d (%d..%d) ~%d\n", fps, fps_min, fps_max, fps_average / fps_counted );
			fps = 0;

			sleep = lib_sys_uptime() + 1000;
		}
	}

	// end of execution
	return 0;
}
