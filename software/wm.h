/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	WM
	#define	WM

	#define	WM_OBJECT_NAME_length		LIB_INTERFACE_NAME_length

	#define	WM_DESCRIPTOR_FLAG_flush	0b0000000000000001
	#define	WM_DESCRIPTOR_FLAG_minimize	0b0000000000000010

	#define	WM_IPC_FUNCTION_create		0
	#define	WM_IPC_FUNCTION_properties	1
	#define	WM_IPC_FUNCTION_rename		2
	#define	WM_IPC_FUNCTION_close		3

	// must be divisible by uint32_t
	struct	WM_STRUCTURE_DESCRIPTOR {
		uint8_t				flags;
		uint16_t			reserved;
		uint8_t				id;
		uint16_t			pointer_x;
		uint16_t			pointer_y;
		uint16_t			x;
		uint16_t			y;
		uint16_t			width;
		uint16_t			height;
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_IPC {
		uint8_t				type;
		uint8_t				function;
		uint8_t				align[ 6 ];
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_IPC_WINDOW_create {
		struct WM_STRUCTURE_IPC		ipc;
		uint8_t				id;
		int16_t				x;
		int16_t				y;
		uint16_t			width;
		uint16_t			height;
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_IPC_WINDOW_properties {
		struct WM_STRUCTURE_IPC		ipc;
		uint8_t				id;
		struct WM_STRUCTURE_DESCRIPTOR	*descriptor;
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_IPC_WINDOW_rename {
		struct WM_STRUCTURE_IPC		ipc;
		uint8_t				id;
		uint8_t				length;
		uint8_t				name[ WM_OBJECT_NAME_length ];
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_IPC_WINDOW_close {
		struct WM_STRUCTURE_IPC		ipc;
		uint8_t				id;
	} __attribute__( (packed) );
#endif