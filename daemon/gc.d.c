/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#define	DAEMON

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/sys.h"
	// kernel --------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/page.h"
	#include	"../kernel/task.h"
	//======================================================================

void child( struct KERNEL_STRUCTURE *kernel, uint64_t pid ) {
	// find all childs of parent
	for( uint64_t i = 0; i < KERNEL_TASK_limit; i++ ) {
		// this task belongs to parent?
		if( kernel -> task_queue_address[ i ].flags && kernel -> task_queue_address[ i ].pid_parent == pid ) {
			// close all child processes belonging to this parent
			child( kernel, kernel -> task_queue_address[ i ].pid );
		
			// mark task as not active anymore
			kernel -> task_queue_address[ i ].flags &= ~KERNEL_TASK_FLAG_active;

			// wait for release
			while( kernel -> task_queue_address[ i ].flags & KERNEL_TASK_FLAG_exec );

			// mark task as closed
			kernel -> task_queue_address[ i ].flags |= KERNEL_TASK_FLAG_closed;
		}
	}
}

void entry( struct KERNEL_STRUCTURE *kernel ) {
	// infinite loop :)
	while( TRUE ) {
		// search in task queue for closed tasks
		for( uint64_t i = 0; i < KERNEL_TASK_limit; i++ ) {
			// task closed?
			if( kernel -> task_queue_address[ i ].flags & KERNEL_TASK_FLAG_closed ) {
				// close all child processes belonging to parent
				child( kernel, kernel -> task_queue_address[ i ].pid );

				// release streams
				kernel -> stream_release( kernel -> task_queue_address[ i ].stream_in );
				kernel -> stream_release( kernel -> task_queue_address[ i ].stream_out );

				// if task marked as thread
				if( kernel -> task_queue_address[ i ].flags & KERNEL_TASK_FLAG_thread ) {
					// remove redundant entries in threads paging table
					uint64_t *pml4 = (uint64_t *) (kernel -> task_queue_address[ i ].cr3 | KERNEL_PAGE_mirror);
					for( uint16_t p = 0; p < KERNEL_PAGE_SIZE_entry; p++ )
						// redundant entry?
						if( p != KERNEL_PAGE_ENTRY_stack && p != KERNEL_PAGE_ENTRY_stack_context ) pml4[ p ] = EMPTY;
				} else
					// release memory map
					kernel -> memory_release( (uintptr_t) kernel -> task_queue_address[ i ].memory_map, MACRO_PAGE_ALIGN_UP( kernel -> page_limit >> STATIC_DIVIDE_BY_8_shift ) >> STATIC_PAGE_SIZE_shift );

				// release paging structure and page itself
				kernel -> page_deconstruction( (uintptr_t *) kernel -> task_queue_address[ i ].cr3 );

				// task released
				kernel -> task_queue_address[ i ].flags = EMPTY;

				// task decommisioned
				kernel -> task_count--;
			}
		}
	}
}