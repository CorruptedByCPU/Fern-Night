/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../../default.h"
	// library -------------------------------------------------------------
	#include	"sys.h"
	//======================================================================

#define	LIB_MATH_PI (3.14159265358979323846264338327950288419716939937510)
#define	LIB_MATH_PI_2 (LIB_MATH_PI / 2.0f)
#define	LIB_MATH_PI_M_2 (6.28318530717958623199592693708837032318115234375000)

int lib_math_double_compare( double f1, double f2 ) {
	double precision = 0.00000000000000000001;

	if( (f1 - precision) < f2 ) return -1;
	else if( (f1 + precision) > f2 ) return 1;

	return 0;
}

double lib_math_cos( double x ) {
	// keep in range
	x = fmod( x, 360.0f );
	if(x > 180.0f) x -= 360.0f;

	// convert to radians
	x = x * LIB_MATH_PI / 180.0f;

	if( x < 0.0f ) x = -x;

	if( 0 <= lib_math_double_compare( x, LIB_MATH_PI_M_2 ) ) do {
		x -= LIB_MATH_PI_M_2;
	} while( 0 <= lib_math_double_compare( x, LIB_MATH_PI_M_2 ) );

	if( (0 <= lib_math_double_compare( x, LIB_MATH_PI ) ) && ( -1 == lib_math_double_compare( x, LIB_MATH_PI_M_2 )) ) {
		x -= LIB_MATH_PI;
		
		return ((-1) * (1.0f - (x * x / 2.0f) * (1.0f - (x * x / 12.0f) * (1.0f - ( x * x / 30.0f) * (1.0f - ( x * x / 56.0f ) * (1.0f - ( x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - ( x * x / 182.0f)))))))));
	}

	return 1.0f - (x * x / 2.0f) * ( 1.0f - (x * x / 12.0f) * ( 1.0f - (x * x / 30.0f) * (1.0f - (x * x / 56.0f) * (1.0f - ( x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - ( x * x / 182.0f)))))));
}

double lib_math_sin( double x ) { return lib_math_cos( x - 90 ); }

double lib_math_tan( double x ) { return lib_math_sin( x ) / lib_math_cos( x ); }

double lib_math_ctan( double x ) { return 1.0f / lib_math_tan( x ); }