/*=============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
=============================================================================*/

#ifndef	KERNEL_STREAM
	#define	KERNEL_STREAM

	#define	KERNEL_STREAM_limit		(KERNEL_TASK_limit << STATIC_MULTIPLE_BY_2_shift)

	struct	KERNEL_STREAM_STRUCTURE {
		uint8_t		*base_address;
		uint16_t	start;
		uint16_t	end;
		uint16_t	free;
		uint8_t		lock;
		volatile uint8_t		flags;
		uint64_t	count;
		uint8_t		meta[ LIB_SYS_STREAM_META_LENGTH_byte ];
	};

	struct KERNEL_STREAM_STRUCTURE *kernel_stream();
	uint8_t kernel_stream_get( uint8_t *meta, uint8_t direction );
	uint64_t kernel_stream_in( uint8_t *data );
	void kernel_stream_out( uint8_t *string, uint64_t length );
	void kernel_stream_out_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
	void kernel_stream_release( struct KERNEL_STREAM_STRUCTURE *stream );
	void kernel_stream_set( uint8_t direction, uint8_t *meta );
#endif
