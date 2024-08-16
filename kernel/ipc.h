/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IPC
	#define	KERNEL_IPC

	#define	KERNEL_IPC_limit	512
	#define	KERNEL_IPC_ttl	100	// ms

	void kernel_ipc_send( uint64_t pid, uint8_t *data );
	uint8_t kernel_ipc_receive( struct STD_IPC_STRUCTURE *message );
#endif