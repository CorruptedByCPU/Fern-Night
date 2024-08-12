/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_stream() {
	// prepare the space for the stream space
	kernel -> stream_base_address = (struct KERNEL_STREAM_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_STREAM_limit * sizeof( struct KERNEL_TASK_STRUCTURE ) ) >> STATIC_PAGE_SIZE_shift );

	// prepare streams for kernel process
	struct KERNEL_STREAM_STRUCTURE *stream = kernel_stream();

	// as a kernel, both streams are of type null
	stream -> flags = LIB_SYS_STREAM_FLAG_null;

	// insert streams into kernel task
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();
	task -> stream_out = stream;
	task -> stream_in = stream;
}