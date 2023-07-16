        typedef	struct VECTOR2D {
                int64_t	x;
                int64_t	y;
        } vector2d;

	struct FACE {
		vector3f	p[ 3 ];
		double		z_depth;
		uint32_t	color;
	};

	struct MATRIX {
		double		c[ 4 ][ 4 ];
	};

MACRO_IMPORT_FILE_AS_STRING( json, "software/3d/interface.json" );
struct LIB_INTERFACE_STRUCTURE interface = { (uintptr_t) &json };

uint32_t *workbench;

struct MATRIX mat2D;

struct FACE *object;
uint64_t object_faces = EMPTY;

uint64_t fps = 0;
uint64_t fps_min = -1;
uint64_t fps_max = 1;
uint64_t fps_average;
uint64_t fps_counted;

uint64_t sleep;

double rotate_x = 0.0f;
double rotate_y = 0.0f;
double rotate_z = 0.0f;

double scale = 0.0f;

double move_x = 0.0f;
double move_y = 0.0f;
double move_z = 0.0f;

double camera_dx = 0.0f;
double camera_dy = 0.0f;
double camera_dz = 0.0f;

void init( uint64_t argc, uint8_t *argv[] ) {
	// load selected file
	lib_sys_storage_read_file( &storage );

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

	object = (struct FACE *) malloc( sizeof( struct FACE ) * tmp_f );
	object_faces = 0;

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
			point[ point_count ].w = 1.0f;

			point_count++;
		}

		// parse faces (triangles)
		if( line[ 0 ] == 'f' ) {
			// set point at first value
			uint8_t *v = (uint8_t *) &line[ 2 ];
			uint64_t vl = lib_string_length_scope_digit( v );

			// first point of triangle
			int64_t x = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
			object[ object_faces ].p[ 0 ] = point[ x ];

			// set pointer at second value
			v += vl + 1;
			vl = lib_string_length_scope_digit( v );

			// second point of triangle
			int64_t y = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
			object[ object_faces ].p[ 1 ] = point[ y ];

			// set pointer at second value
			v += vl + 1;
			vl = lib_string_length_scope_digit( v );

			// third point of triangle
			int64_t z = lib_string_to_integer( v, STATIC_NUMBER_SYSTEM_decimal );
			object[ object_faces ].p[ 2 ] = point[ z ];

			object_faces++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;
	}

	free( point );

	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &interface );

	workbench = (uint32_t *) lib_sys_memory_alloc( (interface.width * interface.height) << STATIC_VIDEO_DEPTH_shift );

	sleep = lib_sys_uptime() + 1000;
}

struct MATRIX lib_rgl_return_matrix_identity() {
	struct MATRIX matrix = { 0.0f };

	matrix.c[ 0 ][ 0 ] = 1.0f;
	matrix.c[ 1 ][ 1 ] = 1.0f;
	matrix.c[ 2 ][ 2 ] = 1.0f;
	matrix.c[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_rotate_x( double angle ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.c[ 1 ][ 1 ] = lib_math_cos( angle );
	matrix.c[ 1 ][ 2 ] = -lib_math_sin( angle );
	matrix.c[ 2 ][ 1 ] = lib_math_sin( angle );
	matrix.c[ 2 ][ 2 ] = lib_math_cos( angle );

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_rotate_y( double angle ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.c[ 0 ][ 0 ] = lib_math_cos( angle );
	matrix.c[ 0 ][ 2 ] = lib_math_sin( angle );
	matrix.c[ 2 ][ 0 ] = -lib_math_sin( angle );
	matrix.c[ 2 ][ 2 ] = lib_math_cos( angle );

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_rotate_z( double angle ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.c[ 0 ][ 0 ] = lib_math_cos( angle );
	matrix.c[ 0 ][ 1 ] = -lib_math_sin( angle );
	matrix.c[ 1 ][ 0 ] = lib_math_sin( angle );
	matrix.c[ 1 ][ 1 ] = lib_math_cos( angle );

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_translate( double x, double y, double z ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.c[ 3 ][ 0 ] = x;
	matrix.c[ 3 ][ 1 ] = y;
	matrix.c[ 3 ][ 2 ] = z;

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_scale( double x, double y, double z ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.c[ 0 ][ 0 ] = x;
	matrix.c[ 1 ][ 1 ] = y;
	matrix.c[ 2 ][ 2 ] = z;

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_view( double v, double n, double f, uint64_t width, uint64_t height ) {
	struct MATRIX matrix = lib_rgl_return_matrix_identity();

	double r = 1.0f / lib_math_tan( (v * 0.5f) / (LIB_MATH_PI * 180.0f) );

	matrix.c[ 0 ][ 0 ] = r;
	matrix.c[ 1 ][ 1 ] = r;
	matrix.c[ 2 ][ 2 ] = -(f / (f - n));
	matrix.c[ 2 ][ 3 ] = 1.0f;
	matrix.c[ 3 ][ 2 ] = -(f * n) / (f - n);

	return matrix;
}

vector3f lib_rgl_multiply_vector( vector3f v, struct MATRIX matrix ) {
	vector3f vector;

	vector.x = (v.x * matrix.c[ 0 ][ 0 ]) + (v.y * matrix.c[ 1 ][ 0 ]) + (v.z * matrix.c[ 2 ][ 0 ]) + (v.w * matrix.c[ 3 ][ 0 ]);
	vector.y = (v.x * matrix.c[ 0 ][ 1 ]) + (v.y * matrix.c[ 1 ][ 1 ]) + (v.z * matrix.c[ 2 ][ 1 ]) + (v.w * matrix.c[ 3 ][ 1 ]);
	vector.z = (v.x * matrix.c[ 0 ][ 2 ]) + (v.y * matrix.c[ 1 ][ 2 ]) + (v.z * matrix.c[ 2 ][ 2 ]) + (v.w * matrix.c[ 3 ][ 2 ]);
	vector.w = (v.x * matrix.c[ 0 ][ 3 ]) + (v.y * matrix.c[ 1 ][ 3 ]) + (v.z * matrix.c[ 2 ][ 3 ]) + (v.w * matrix.c[ 3 ][ 3 ]);

	vector.x /= vector.w;
	vector.y /= vector.w;
	vector.z /= vector.w;

	return vector;
}

void lib_rgl_line( int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color ) {
	int64_t dx = abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1;
	int64_t dy = -abs( y1 - y0 ), sy = y0 < y1 ? 1 : -1; 
	int64_t err = dx + dy, e2;

	for(;;) {
		if( x0 >= 0 && x0 < interface.width && y0 >= 0 && y0 < interface.height )
			workbench[ (y0 * interface.width) + x0 ] = color;

		if (x0 == x1 && y0 == y1) break;

		e2 = 2 * err;
	
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void lib_rgl_flush() {
	// workbench index
	uint64_t w = EMPTY;

	// synchronize standard output with workbench
	for( uint64_t y = 0; y < interface.height; y++ )
		for( uint64_t x = 0; x < interface.width; x++ )
			interface.base_address[ (y * interface.width) + x ] = workbench[ w++ ];
}

inline uint8_t lib_rgl_edge( vector2d a, vector2d b, vector2d c ) {
	if( (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < 0 ) return TRUE;
	return FALSE;
}

void lib_rgl_face( struct FACE *face ) {
	vector2d p0 = { (int64_t) face -> p[ 0 ].x, (int64_t) face -> p[ 0 ].y };
	vector2d p1 = { (int64_t) face -> p[ 1 ].x, (int64_t) face -> p[ 1 ].y };
	vector2d p2 = { (int64_t) face -> p[ 2 ].x, (int64_t) face -> p[ 2 ].y };

	int64_t left = p0.x;
	if( p1.x < left ) left = p1.x;
	if( p2.x < left ) left = p2.x;

	int64_t top = p0.y;
	if( p1.y < top ) top = p1.y;
	if( p2.y < top ) top = p2.y;

	int64_t right = p0.x;
	if( p1.x > right ) right = p1.x;
	if( p2.x > right ) right = p2.x;

	int64_t bottom = p0.y;
	if( p1.y > bottom ) bottom = p1.y;
	if( p2.y > bottom ) bottom = p2.y;

	vector2d p;

	for( p.y = top; p.y <= bottom; p.y++ )
		for( p.x = left; p.x <= right; p.x++ ) {
			// pixel is inside workbench?
			int64_t x = p.x + (interface.width / 2);
			int64_t y = -p.y + (interface.height / 2);
			if( x < 0 || x >= interface.width ) continue;	// no
			if( y < 0 || y >= interface.height ) continue;	// no

			// pixel inside triangle?
			if( ! lib_rgl_edge( p1, p2, p ) ) continue;
			if( ! lib_rgl_edge( p2, p0, p ) ) continue;
			if( ! lib_rgl_edge( p0, p1, p ) ) continue;

			// draw it
			workbench[ (y * interface.width) + x ] = face -> color;
		}

	lib_rgl_line( p0.x + (interface.width / 2), -p0.y + (interface.height / 2), p1.x + (interface.width / 2), -p1.y + (interface.height / 2), face -> color );
	lib_rgl_line( p1.x + (interface.width / 2), -p1.y + (interface.height / 2), p2.x + (interface.width / 2), -p2.y + (interface.height / 2), face -> color );
	lib_rgl_line( p2.x + (interface.width / 2), -p2.y + (interface.height / 2), p0.x + (interface.width / 2), -p0.y + (interface.height / 2), face -> color );
}

uint64_t lib_rgl_partition( struct FACE **face, uint64_t low, uint64_t high ) {
	double pivot = face[ high ] -> z_depth;

	uint64_t i = (low - 1);

	for( uint64_t j = low; j < high; j++ ) {
		if( face[ j ] -> z_depth < pivot ) {
			i++;

			struct FACE *tmp = face[ i ];
			face[ i ] = face[ j ];
			face[ j ] = tmp;
		}
	}

	struct FACE *tmp = face[ i + 1 ];
	face[ i + 1 ] = face[ high ];
	face[ high ] = tmp;

	return (i + 1);
}

void lib_rgl_sort_quick( struct FACE **face, uint64_t low, uint64_t high ) {
	if (low < high) {
		uint64_t pi = lib_rgl_partition( face, low, high );

		lib_rgl_sort_quick( face, low, pi - 1 );
		lib_rgl_sort_quick( face, pi + 1, high );
	}
}

vector3f lib_rgl_vector_substract( vector3f from, vector3f substract ) {
	vector3f tmp;

	tmp.x = from.x - substract.x;
	tmp.y = from.y - substract.y;
	tmp.z = from.z - substract.z;

	return tmp;
}

vector3f lib_rgl_vector_add( vector3f v1, vector3f v2 ) {
	return (vector3f) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

vector3f lib_rgl_vector_multiple( vector3f v, double m ) {
	return (vector3f) { v.x * m, v.y * m, v.z * m };
}

vector3f lib_rgl_vector_product_cross( vector3f v0, vector3f v1 ) {
	vector3f tmp;

	tmp.x = (v0.y * v1.z) - (v0.z * v1.y);
	tmp.y = (v0.z * v1.x) - (v0.x * v1.z);
	tmp.z = (v0.x * v1.y) - (v0.y * v1.x);

	return tmp;
}

double lib_rgl_vector_product_dot( vector3f v0, vector3f v1 ) {
	return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}

double lib_rgl_vector_length( vector3f v ) {
	return (double) sqrtf( (float) lib_rgl_vector_product_dot( v, v ) );
}

vector3f lib_rgl_vector_normalize( vector3f v ) {
	double length = lib_rgl_vector_length( v );

	v.x /= length;
	v.y /= length;
	v.z /= length;

	return v;
}

struct MATRIX lib_rgl_multiply_matrix( struct MATRIX this, struct MATRIX via ) {
	struct MATRIX matrix;

	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			matrix.c[ x ][ y ] = this.c[ x ][ 0 ] * via.c[ 0 ][ y ] + this.c[ x ][ 1 ] * via.c[ 1 ][ y ] + this.c[ x ][ 2 ] * via.c[ 2 ][ y ] + this.c[ x ][ 3 ] * via.c[ 3 ][ y ];

	return matrix;
}

struct MATRIX lib_rgl_return_matrix_world() {
	struct MATRIX Y = lib_rgl_return_matrix_rotate_y( rotate_y );
	struct MATRIX X = lib_rgl_return_matrix_rotate_x( rotate_x );
	struct MATRIX XY = lib_rgl_multiply_matrix( Y, X );
	struct MATRIX matW = lib_rgl_multiply_matrix( XY, lib_rgl_return_matrix_translate( move.x, move.y, move.z ) );

	return matW;
}