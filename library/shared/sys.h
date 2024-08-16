/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_SYS
	#define	LIB_SYS

	#define	STD_SHIFT_256					8

	#define	LIB_SYS_REQUEST_EXIT				0x00
	#define	LIB_SYS_REQUEST_FRAMEBUFFER			0x01
	#define	LIB_SYS_REQUEST_MEMORY_ALLOC			0x02
	#define	LIB_SYS_REQUEST_MEMORY_RELEASE			0x03
	#define	LIB_SYS_REQUEST_TASK_PID			0x04
	#define	LIB_SYS_REQUEST_MOUSE				0x05
	#define	LIB_SYS_REQUEST_STORAGE_READ_FILE		0x06
	#define	LIB_SYS_REQUEST_EXEC				0x07
	#define	LIB_SYS_REQUEST_IPC_SEND			0x08
	#define	LIB_SYS_REQUEST_IPC_RECEIVE			0x09
	#define	LIB_SYS_REQUEST_MEMORY_SHARE			0x0A
	#define	LIB_SYS_REQUEST_KEYBOARD			0x0B
	#define	LIB_SYS_REQUEST_TASK_STATUS			0x0C
	#define	LIB_SYS_REQUEST_STREAM_OUT			0x0D
	#define	LIB_SYS_REQUEST_STREAM_IN			0x0E
	#define	LIB_SYS_REQUEST_SERIAL_CHAR			0x0F
	#define	LIB_SYS_REQUEST_SERIAL_STRING			0x10
	#define	LIB_SYS_REQUEST_SERIAL_VALUE			0x11
	#define	LIB_SYS_REQUEST_TIME				0x12
	#define	LIB_SYS_REQUEST_STREAM_SET			0x13
	#define	LIB_SYS_REQUEST_STREAM_GET			0x14
	#define	LIB_SYS_REQUEST_SLEEP				0x15
	#define	LIB_SYS_REQUEST_UPTIME				0x16
	#define	LIB_SYS_REQUEST_STREAM_OUT_VALUE		0x17
	#define	LIB_SYS_REQUEST_TASK				0x18
	#define	LIB_SYS_REQUEST_MEMORY				0x19
	#define	LIB_SYS_REQUEST_THREAD				0x1A

	//-----------------------------------------------------------------------------

	#define	LIB_SYS_ERROR_VFS_PATH				-1
	#define	LIB_SYS_ERROR_VFS_FILE_NOT_EXIST		-2
	#define	LIB_SYS_ERROR_VFS_IS_NOT_DIRECTORY		-3
	#define	LIB_SYS_ERROR_MEMORY_no_enough			-4
	#define	LIB_SYS_ERROR_MEMORY_invalid_address		-5
	#define LIB_SYS_ERROR_NETWORK_socket_unavailable	-6
	#define LIB_SYS_ERROR_NETWORK_port_unavailable		-7
	#define	LIB_SYS_ERROR_EXEC_unknown_file_type		-8

	//-----------------------------------------------------------------------------

	#define	LIB_SYS_TASK_NAME_length			255

	#define	LIB_SYS_TASK_FLAG_active			0b0000000000000001
	#define	LIB_SYS_TASK_FLAG_closed			0b0000000000000010
	#define	LIB_SYS_TASK_FLAG_daemon			0b0000000000000100
	#define	LIB_SYS_TASK_FLAG_exec				0b0000000000001000
	#define	LIB_SYS_TASK_FLAG_secured			0b0000000000010000
	#define	LIB_SYS_TASK_FLAG_thread			0b0000000000100000
	#define	LIB_SYS_TASK_FLAG_sleep				0b0000000100000000
	#define	LIB_SYS_TASK_FLAG_init				0b0000010000000000

	struct	LIB_SYS_STRUCTURE_TASK {
		uint64_t	pid;
		uint64_t	pid_parent;
		uint64_t	sleep;
		uint64_t	page;
		uint16_t	flags;
		uint8_t		length;
		uint8_t		name[ LIB_SYS_TASK_NAME_length ];
	} __attribute__( (packed) );

	//-----------------------------------------------------------------------------

	struct	LIB_SYS_STRUCTURE_MEMORY {
		uint64_t	total;
		uint64_t	available;
	} __attribute__( (packed) );

	//-----------------------------------------------------------------------------

	#define	LIB_SYS_NETWORK_PROTOCOL_udp			0x11

	//-----------------------------------------------------------------------------

	#define	LIB_SYS_STREAM_SIZE_byte			4096

	#define	LIB_SYS_STREAM_FLOW_out_to_parent_out		0b00000001
	#define	LIB_SYS_STREAM_FLOW_out_to_parent_in		0b00000010
	#define	LIB_SYS_STREAM_FLOW_out_to_in			0b00000100

	#define LIB_SYS_STREAM_META_LENGTH_byte			0x08

	#define	LIB_SYS_STREAM_FLAG_data			0b00000001
	#define	LIB_SYS_STREAM_FLAG_undefinied			0b00000010
	#define	LIB_SYS_STREAM_FLAG_null			0b01000000
	#define	LIB_SYS_STREAM_FLAG_closed			0b10000000

	#define	LIB_SYS_STREAM_out				0b00000001
	#define	LIB_SYS_STREAM_in				0b00000010

	//-----------------------------------------------------------------------------

	struct	LIB_SYS_STRUCTURE_MOUSE {
		uint16_t	x;
		uint16_t	y;
		// int64_t		x_absolute;
		// int64_t		y_absolute;
		uint8_t		status;
	} __attribute__( (packed) );

	struct	LIB_SYS_STRUCTURE_STORAGE {
		uintptr_t	address;
		uint64_t	size_byte;
		uint8_t		length;
		uint8_t		name[ 255 ];
	} __attribute__( (packed) );

	//-----------------------------------------------------------------------------

	#define	STD_IPC_SIZE_byte			40

	#define	LIB_SYS_IPC_TYPE_ANY				0x00
	#define	LIB_SYS_IPC_TYPE_KEYBOARD			0x01
	#define	LIB_SYS_IPC_TYPE_MOUSE				0x02
	#define	LIB_SYS_IPC_TYPE_WINDOW_MANAGER			0x03	// message contains data: window manager

	struct	LIB_SYS_STRUCTURE_FRAMEBUFFER {
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint32_t	*base_address;
		uint64_t	scanline_byte;
		uint64_t	pid;
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE {
		uint64_t	ttl;
		uint64_t	source;
		uint64_t	target;
		uint8_t		data[ STD_IPC_SIZE_byte ];
	} __attribute__( (packed) );

	#define	LIB_SYS_device_mouse_status_left_mouse_button	0b00000001
	#define	LIB_SYS_device_mouse_status_right_mouse_button	0b00000010

	struct	STD_IPC_STRUCTURE_DEFAULT {
		uint8_t		type;
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE_KEYBOARD {
		uint8_t		type;
		uint16_t	key;
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE_MOUSE {
		uint8_t		type;
		uint8_t		status;
	} __attribute__( (packed) );

	//-----------------------------------------------------------------------------

	#define	LIB_SYS_INTERFACE_HEADER_height_in_pixel	20

	//-----------------------------------------------------------------------------

	inline void lib_sys_return_empty();
	inline uint8_t lib_sys_return_bool();
	inline int64_t lib_sys_return_value();
	inline uint64_t lib_sys_return_value_unsigned();
	inline uintptr_t lib_sys_return_pointer();
	inline void lib_sys_int_return_empty();
	inline uint8_t lib_sys_int_return_bool();
	inline int64_t lib_sys_int_return_value();
	inline uint64_t lib_sys_int_return_value_unsigned();
	inline uintptr_t lib_sys_int_return_pointer();
	void lib_sys_ipc_send( uint64_t pid, uint8_t *message );
	uint8_t lib_sys_ipc_receive( struct STD_IPC_STRUCTURE *message, uint8_t type );
	void lib_sys_int_ipc_send( uint64_t pid, uint8_t *message );
	uint8_t lib_sys_int_ipc_receive( struct STD_IPC_STRUCTURE *message, uint8_t type );
	void lib_sys_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer );
	void lib_sys_int_framebuffer( struct LIB_SYS_STRUCTURE_FRAMEBUFFER *framebuffer );
	uint64_t lib_sys_task_pid();
	uint64_t lib_sys_int_task_pid();
	void lib_sys_storage_read_file( struct LIB_SYS_STRUCTURE_STORAGE *storage );
	void lib_sys_int_storage_read_file( struct LIB_SYS_STRUCTURE_STORAGE *storage );
	void lib_sys_mouse( uintptr_t mouse );
	void lib_sys_int_driver_mouse( uintptr_t mouse );
	uintptr_t lib_sys_memory_alloc( uint64_t bytes );
	void lib_sys_memory_release( uintptr_t address, uint64_t bytes );
	uintptr_t lib_sys_memory_share( uintptr_t address, uint64_t bytes, uint64_t pid );
	uintptr_t lib_sys_int_memory_alloc( uint64_t bytes );
	void lib_sys_int_memory_release( uintptr_t address, uint64_t bytes );
	uintptr_t lib_sys_int_memory_share( uintptr_t address, uint64_t bytes, uint64_t pid );
	uint64_t lib_sys_exec( uint8_t *path, uint16_t length, uint8_t stream );
	uint64_t lib_sys_int_exec( uint8_t *path, uint16_t length, uint8_t stream );
	void lib_sys_exit();
	void lib_sys_window_create( uintptr_t window );
	void lib_sys_int_window_create( uintptr_t window );
	void *malloc( size_t byte );
	void free( void *address );
	void *realloc( uintptr_t source, size_t byte );
	void lib_sys_stream_out( uint8_t *string, uint64_t length );
	uint64_t lib_sys_stream_in( uint8_t *cache );
	void lib_sys_int_stream_out( uint8_t *string, uint64_t length );
	uint64_t lib_sys_int_stream_in( uint8_t *cache );
	uint16_t lib_sys_task_status( uint64_t pid );
	uint16_t lib_sys_int_task_status( uint64_t pid );
	uint16_t lib_sys_keyboard();
	uint16_t lib_sys_int_keyboard();
	void lib_sys_serial_char( uint8_t character );
	void lib_sys_serial_string( uint8_t *string, uint64_t length );
	void lib_sys_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t negative );
	void lib_sys_int_serial_char( uint8_t character );
	void lib_sys_int_serial_string( uint8_t *string, uint64_t length );
	void lib_sys_int_serial_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t negative );
	uint64_t lib_sys_time();
	uint64_t lib_sys_int_time();
	void	lib_sys_stream_set( uint8_t *meta, uint8_t stream );
	uint8_t lib_sys_stream_get( uint8_t *meta, uint8_t stream );
	void	lib_sys_int_stream_set( uint8_t *meta, uint8_t stream );
	uint8_t lib_sys_int_stream_get( uint8_t *meta, uint8_t stream );
	void print( const char *string );
	void printf( const char *string, ... );
	uint64_t lib_sys_time();
	void lib_sys_sleep( uint64_t ticks );
	uint64_t lib_sys_uptime();
	uint64_t lib_sys_int_time();
	void lib_sys_int_sleep( uint64_t ticks );
	uint64_t lib_sys_int_uptime();
	void lib_sys_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
	void lib_sys_int_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
	void exit();
	uintptr_t lib_sys_task();
	uintptr_t lib_sys_int_task();
	void lib_sys_memory( struct LIB_SYS_STRUCTURE_MEMORY *memory );
	void log( const char *string, ... );
	uint16_t getkey();
	uint64_t pow( uint64_t base, uint64_t exponent );
	uint64_t lib_sys_thread( uintptr_t function, uint8_t *string, uint8_t length );
	uint64_t lib_sys_int_thread( uintptr_t function, uint8_t *string, uint8_t length );
	uint64_t abs( int64_t i );
	double fmod( double x, double y );
	float sqrtf( float x );
	void *memset( void *bufptr, int64_t value, size_t size );
	void *memcpy( void *restrict dstptr, const void *restrict srcptr, size_t size);
	double strtof( uint8_t *string, uint64_t length );
	double maxf( double first, double second );

	#ifndef	DAEMON
		// initial function of every program
		extern int64_t _main( uint64_t argc, uint8_t *argv[] );

		struct	LIB_SYS_ENTRY {
			uint64_t	length;
			uint8_t		*string;
		} __attribute__( (packed) );

		void entry( struct LIB_SYS_ENTRY arg ) {
			// sad hack :|
			__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

			// amount of args inside string
			uint64_t argc = 0;
			for( uint64_t i = 0; i < arg.length; i++ ) if( arg.string[ i ] != STATIC_ASCII_SPACE ) { argc++; for( ; i < arg.length; i++ ) if( arg.string[ i ] == STATIC_ASCII_SPACE ) break; }

			// allocate memory for argv vectors
			uint8_t *argv[ argc ];

			// insert pointers to every argument inside string
			uint64_t a = EMPTY;
			for( uint64_t c = 0; c < arg.length; c++ ) {
				// arg?
				if( arg.string[ c ] != STATIC_ASCII_SPACE ) {
					// save pointer to argument
					argv[ a++ ] = (uint8_t *) &arg.string[ c ];

					// search for next arg
					for( ; c < arg.length; c++ ) if( arg.string[ c ] == STATIC_ASCII_SPACE ) { arg.string[ c ] = STATIC_ASCII_TERMINATOR; break; }
				}
			}

			// execute process flow
			int64_t result;	// initialize local variable
			__asm__ volatile( "call _main" : "=a" (result) : "D" (argc), "S" (argv) );

			// execute leave out routine
			__asm__ volatile( "" :: "a" (LIB_SYS_REQUEST_EXIT) );
			__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
		}
	#endif
#endif