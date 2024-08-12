/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// driver --------------------------------------------------------------
	#include	"../kernel/driver/ps2.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/sys.h"
	#include	"../library/shared/interface.c"
	#include	"../library/shared/rgl.c"
	// internal ------------------------------------------------------------
	#include	"3d/config.h"
	#include	"3d/data.c"
	//======================================================================

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize engine
	init( argc, argv );

	// main loop
	while( TRUE ) {
		// check for events from interface
		lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &interface );

		// recieve key
		uint16_t key = getkey();

		if( key == 't' ) { if( rgl -> wireless ) rgl -> wireless = FALSE; else rgl -> wireless = TRUE; }

		if( key == 'q' ) lib_sys_exit();

		// release AP time
		lib_sys_sleep( 1 );
	}

	// end of execution
	return 0;
}

void close( void ) {
	// end of execution
	lib_sys_exit();
}

void engine( void ) {
	// sad hack :|
	__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

	// struct LIB_RGL_STRUCTURE_TRIANGLE triangle;
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * object_count );
	struct LIB_RGL_STRUCTURE_TRIANGLE *ready = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * object_count );
	struct LIB_RGL_STRUCTURE_TRIANGLE **sorted = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * object_count );

	uint64_t frames = 0;
	uint64_t min = -1;
	uint64_t max = 0;
	uint64_t wait = lib_sys_uptime();
	uint64_t avarage = 0;
	uint64_t counted = 0;

	while( TRUE ) {
		// clean space with default background color
		lib_rgl_clean( rgl );

		// next angle
		alpha += 0.5f;

		uint64_t inserted = EMPTY;

		struct LIB_RGL_STRUCTURE_MATRIX z_rotate = lib_rgl_return_matrix_rotate_z( alpha / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX x_rotate = lib_rgl_return_matrix_rotate_x( 0.0f );
		struct LIB_RGL_STRUCTURE_MATRIX y_rotate = lib_rgl_return_matrix_rotate_y( alpha );

		struct LIB_RGL_STRUCTURE_MATRIX translate = lib_rgl_return_matrix_translate( move_x, move_y, move_z );

		struct LIB_RGL_STRUCTURE_MATRIX world;
		world = lib_rgl_multiply_matrix( (struct LIB_RGL_STRUCTURE_MATRIX *) &z_rotate, (struct LIB_RGL_STRUCTURE_MATRIX *) &x_rotate );
		world = lib_rgl_multiply_matrix( (struct LIB_RGL_STRUCTURE_MATRIX *) &world, (struct LIB_RGL_STRUCTURE_MATRIX *) &y_rotate );
		world = lib_rgl_multiply_matrix( (struct LIB_RGL_STRUCTURE_MATRIX *) &world, (struct LIB_RGL_STRUCTURE_MATRIX *) &translate );



		// // Create "Point At" Matrix for camera
		// vec3d vUp = { 0,1,0 };
		// vec3d vTarget = { 0,0,1 };
		// mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
		// vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		// vTarget = Vector_Add(vCamera, vLookDir);
		// mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// // Make view matrix from camera
		// mat4x4 matView = Matrix_QuickInverse(matCamera);




		for( uint64_t i = 0; i < object_count; i++ ) {
			parse[ i ] = object[ i ];

			lib_rgl_multiply( (struct LIB_RGL_STRUCTURE_TRIANGLE *) &parse[ i ], (struct LIB_RGL_STRUCTURE_MATRIX *) &world );

			if( lib_rgl_projection( rgl, (struct LIB_RGL_STRUCTURE_TRIANGLE *) &parse[ i ] ) ) {
				sorted[ inserted ] = &parse[ i ];
				sorted[ inserted ] -> z_depth = (parse[ i ].point[ 0 ].z + parse[ i ].point[ 1 ].z + parse[ i ].point[ 2 ].z) / 3.0f;
				inserted++;
			}
		}

		lib_rgl_sort_quick( sorted, 1, inserted - 1 );

		for( uint64_t i = 0; i < inserted; i++ )
			// draw triangle
			lib_rgl_triangle( rgl, sorted[ i ] );
			
		// synchronize workbench with window
		lib_rgl_flush( rgl );

		// tell window manager to flush window content
		interface.window_descriptor -> flags |= WM_DESCRIPTOR_FLAG_flush;

		frames++;

		if( lib_sys_uptime() >= wait + 1024 ) {
			wait = lib_sys_uptime();

			if( min > frames ) min = frames;
			if( max < frames ) max = frames;

			avarage += frames;
			counted++;

			printf( "FPS: %d (%d..%d) ~%d\n", frames, min, max, avarage / counted );
			frames = 0;
		}
	}
}

void init( uint64_t argc, uint8_t *argv[] ) {
	// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 10, "ferris.obj" };
	// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 9, "earth.obj" };
	struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 10, "teapot.obj" };
	// struct LIB_SYS_STRUCTURE_STORAGE storage = { EMPTY, EMPTY, 10, "monkey.obj" };

	// if file selected
	if( argc > 1 ) {
		// update file descriptor
		storage.length = lib_string_length( argv[ 1 ] );
		for( uint16_t i = 0; i < storage.length; i++ ) storage.name[ i ] = argv[ 1 ][ i ];
	}

	// load selected file
	lib_sys_storage_read_file( &storage );

	// file loaded?
	if( storage.address ) {	// yes
		uint64_t tmp_v = 0;
		uint64_t tmp_f = 0;

		// calculate amount of vectors and faces
		uint8_t *line = (uint8_t *) storage.address;
		while( lib_string_length_line( line ) ) {
			if( line[ 0 ] == 'v' ) tmp_v++;
			if( line[ 0 ] == 'f' ) tmp_f++;

			// next line from file
			line += lib_string_length_line( line ) + 1;
		}

		vector3f *point = (vector3f *) malloc( sizeof( vector3f ) * ++tmp_v );
		uint64_t point_count = 1;

		object = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * tmp_f );
		object_count = 0;

		// properties of line
		line = (uint8_t *) storage.address;
		while( lib_string_length_line( line ) ) {
			// parse points (vectors)
			if( line[ 0 ] == 'v' ) {
				// set point at first value
				uint8_t *v = (uint8_t *) &line[ 2 ];
				uint64_t vl = lib_string_word( v, lib_string_length_line( line ) );

				// X axis
				point[ point_count ].x = strtof( v, vl );

				// set pointer at second value
				v += vl + 1;
				vl = lib_string_word( v, lib_string_length_line( line ) );

				// Y axis
				point[ point_count ].y = strtof( v, vl );

				// set pointer at third value
				v += vl + 1;
				vl = lib_string_word( v, lib_string_length_line( line ) );

				// Z axis
				point[ point_count ].z = strtof( v, vl );

				// W as default
				point[ point_count++ ].w = 1.0f;
			}

			// parse faces (triangles)
			if( line[ 0 ] == 'f' ) {
				// set point at first value
				uint8_t *v = (uint8_t *) &line[ 2 ];
				uint64_t vl = lib_string_length_scope_digit( v );

				// first point of triangle
				int64_t x = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
				object[ object_count ].point[ 0 ] = point[ x ];

				// set pointer at second value
				v += vl + 1;
				vl = lib_string_length_scope_digit( v );

				// second point of triangle
				int64_t y = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
				object[ object_count ].point[ 1 ] = point[ y ];

				// set pointer at second value
				v += vl + 1;
				vl = lib_string_length_scope_digit( v );

				// third point of triangle
				int64_t z = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
				object[ object_count ].point[ 2 ] = point[ z ];

				object_count++;
			}

			// next line from file
			line += lib_string_length_line( line ) + 1;
		}

		free( point );
	} else {
		// show unrecognized file name
		storage.name[ storage.length ] = STATIC_ASCII_TERMINATOR;
		log( "File '%s' not found.", storage.name );

		// end of program
		lib_sys_exit();
	}

	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &interface );

	// find element with ID:1 and point event function to close();
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *element = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &interface, 1 );
	if( element ) element -> event = (void *) close;

	// Raw Graphics Layer
	rgl = lib_rgl( interface.width - 2, interface.height - LIB_INTERFACE_HEADER_HEIGHT_pixel - 1, interface.width, interface.base_address + (interface.width * LIB_INTERFACE_HEADER_HEIGHT_pixel) + 1 );

	// create new thread
	uint8_t thread[] = "3d -> projection";
	lib_sys_thread( (uintptr_t) &engine, (uint8_t *) &thread, sizeof( thread ) - 1 );
}
