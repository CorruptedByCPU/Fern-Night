/*=============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
=============================================================================*/

struct KERNEL_STREAM_STRUCTURE *kernel_stream() {
	// deny access, only one logical processor at a time
	while( __sync_val_compare_and_swap( &kernel -> stream_semaphore, UNLOCK, LOCK ) );

	// search stream table for free entry
	for( uint64_t i = 0; i < KERNEL_STREAM_limit; i++ ) {
		// closed stream or free entry?
		if( ! kernel -> stream_base_address[ i ].base_address ) {
			// prepare the space for the stream
			kernel -> stream_base_address[ i ].base_address = (uint8_t *) kernel_memory_alloc( KERNEL_STREAM_SIZE_page );

			// there is currently no data in the stream
			kernel -> stream_base_address[ i ].start = EMPTY;
			kernel -> stream_base_address[ i ].end = EMPTY;
			kernel -> stream_base_address[ i ].free = STATIC_PAGE_SIZE_byte;

			// number of processes using the stream
			kernel -> stream_base_address[ i ].count = TRUE;

			// unlock access to functions
			kernel -> stream_semaphore = UNLOCK;

			// return stream id
			return (struct KERNEL_STREAM_STRUCTURE *) &kernel -> stream_base_address[ i ];
		}
	}

	// unlock access to functions
	kernel -> stream_semaphore = UNLOCK;

	// no free entry
	return	EMPTY;
}

void kernel_stream_release( struct KERNEL_STREAM_STRUCTURE *stream ) {
	// only one process used the stream?
	if( ! --stream -> count ) {
		// free up stream space
		kernel_memory_release( (uintptr_t) stream -> base_address, KERNEL_STREAM_SIZE_page );

		// free entry
		stream -> base_address = EMPTY;
	}
}

void kernel_stream_out( uint8_t *string, uint64_t length ) {
	// get the process output stream id
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// data volume supported?
	if( length > LIB_SYS_STREAM_SIZE_byte ) return;	// no

	// stream closed or null?
	if( task -> stream_out -> flags & (LIB_SYS_STREAM_FLAG_closed | LIB_SYS_STREAM_FLAG_null) ) return;	// yes

	// block access to the stream
	while( __sync_val_compare_and_swap( &task -> stream_out -> lock, UNLOCK, LOCK ) );

	// enough space in the stream?
	if( task -> stream_out -> free >= length ) {	// yes
		// current character from string
		uint64_t sent = EMPTY;

		// send data from end marker to end of stream
		if( task -> stream_out -> end >= task -> stream_out -> start ) {
			while( sent != length && task -> stream_out -> end < LIB_SYS_STREAM_SIZE_byte )
				task -> stream_out -> base_address[ task -> stream_out -> end++ ] = string[ sent++ ];

			// data left to be loaded?
			if( sent != length ) 
				// set a new end-of-stream marker
				task -> stream_out -> end = EMPTY;
		}

		// send data from stream start to end marker
		while( sent != length ) task -> stream_out -> base_address[ task -> stream_out -> end++ ] = string[ sent++ ];

		// after operation there will be less space inside stream
		task -> stream_out -> free -= length;

		// set meta flag
		task -> stream_out -> flags |= LIB_SYS_STREAM_FLAG_undefinied;
	}

	// unlock stream access
	task -> stream_out -> lock = UNLOCK;
}

void kernel_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character ) {
	// base of value not supported?
	if( base < 2 || base > 36 ) return;	// yes

	// temporary space for ASCII value
	uint8_t i = 64;
	uint8_t string[ 64 ] = { [0 ... 62] = character, [63] = STATIC_ASCII_DIGIT_0 };

	// prefix cannot be ZERO
	if( ! prefix ) prefix++;

	// if value empty
	if( ! value ) {
		kernel_stream_out( (uint8_t *) &string[ sizeof( string ) - prefix ], prefix );

		return;
	}

	// convert value to single digits
	while( value ) {
		i--;

		// first digit of value
		uint8_t digit = value % base;

		// convert digit to ASCII code
		if( digit < 10 ) string[ i ] = digit + STATIC_ASCII_DIGIT_0;
		else string[ i ] = digit + STATIC_ASCII_DIGIT_7;

		// remove digit from value
		value /= base;
	};

	// complete value with ZERO digits as length of prefix
	if( prefix < i ) i = prefix;

	// display all digits
	kernel_stream_out( (uint8_t *) &string[ i + 1 ], i );
}

uint64_t kernel_stream_in( uint8_t *cache ) {
	// get the process output stream id
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream closed or empty?
	if( task -> stream_in -> flags & LIB_SYS_STREAM_FLAG_closed || task -> stream_in -> free == LIB_SYS_STREAM_SIZE_byte ) return EMPTY;	// tak

	// lock access to the stream
	while( __sync_val_compare_and_swap( &task -> stream_in -> lock, UNLOCK, LOCK ) );

	// size of the processed string
	uint32_t received = EMPTY;

	// send the data from the start marker to the end of the stream
	if( task -> stream_in -> start > task -> stream_in -> end ) {
		while( task -> stream_in -> start < LIB_SYS_STREAM_SIZE_byte )
			cache[ received++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

		// set a new stream start marker
		task -> stream_in -> start = EMPTY;
	}

	// send data from stream start to end marker
	while( task -> stream_in -> start != task -> stream_in -> end )
		cache[ received++ ] = task -> stream_in -> base_address[ task -> stream_in -> start++ ];

	// all flux space free
	task -> stream_in -> free = LIB_SYS_STREAM_SIZE_byte;

	// unlock stream access
	task -> stream_in -> lock = UNLOCK;

	// return amount of transferred data in Bytes
	return received;
}

uint8_t kernel_stream_get( uint8_t *meta, uint8_t direction ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream properties
	struct KERNEL_STREAM_STRUCTURE *stream;

	// which stream?
	if( direction & LIB_SYS_STREAM_in ) stream = (struct KERNEL_STREAM_STRUCTURE *) task -> stream_in;
	else stream = (struct KERNEL_STREAM_STRUCTURE *) task -> stream_out;

	// lock access to the stream
	while( __sync_val_compare_and_swap( &stream -> lock, UNLOCK, LOCK ) );

	// retrieve metadata
	uint8_t *data = (uint8_t *) &stream -> meta;
	for( uint8_t i = 0; i < LIB_SYS_STREAM_META_LENGTH_byte; i++ ) meta[ i ] = data[ i ];

	// unlock access to functions
	stream -> lock = UNLOCK;

	// return stream flags
	return stream -> flags;
}

void kernel_stream_set( uint8_t direction, uint8_t *meta ) {
	// task properties
	struct KERNEL_TASK_STRUCTURE *task = (struct KERNEL_TASK_STRUCTURE *) kernel_task_active();

	// stream properties
	struct KERNEL_STREAM_STRUCTURE *stream;

	// which stream?
	if( direction & LIB_SYS_STREAM_in ) stream = (struct KERNEL_STREAM_STRUCTURE *) task -> stream_in;
	else stream = (struct KERNEL_STREAM_STRUCTURE *) task -> stream_out;

	// lock access to the stream
	while( __sync_val_compare_and_swap( &stream -> lock, UNLOCK, LOCK ) );

	// ignore metadata modification if stream is not empty
	if( stream -> start == stream -> end ) {
		// update metadata, retrieve old
		uint8_t *data = (uint8_t *) &stream -> meta;
		for( uint8_t i = 0; i < LIB_SYS_STREAM_META_LENGTH_byte; i++ ) data[ i ] = meta[ i ];

		// metadata are up to date
		stream -> flags &= ~LIB_SYS_STREAM_FLAG_undefinied;
	}

	// unlock access to functions
	stream -> lock = UNLOCK;
}