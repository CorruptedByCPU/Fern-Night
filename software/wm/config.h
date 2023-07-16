/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#define	WM_OBJECT_limit			255
#define	WM_LIST_limit			(WM_OBJECT_limit + 1)
#define	WM_ZONE_limit			(WM_OBJECT_limit << STATIC_MULTIPLE_BY_4_shift)

#define	WM_OBJECT_FLAG_flush		0b0000000000000001	// the object has been updated
#define	WM_OBJECT_FLAG_visible		0b0000000000000010	// visible object
#define	WM_OBJECT_FLAG_fixed_xy		0b0000000000000100	// movement fixed on X, Y axis
#define	WM_OBJECT_FLAG_fixed_z		0b0000000000001000	// movement fixed on the z axis
#define	WM_OBJECT_FLAG_fragile		0b0000000000010000	// hide object when the action with LMB or PPM occurs
#define	WM_OBJECT_FLAG_arbiter		0b0000000000100000	// overobject
#define	WM_OBJECT_FLAG_hide		0b0000000001000000	// redraw the space under the object
#define	WM_OBJECT_FLAG_init		0b0000000010000000	// the first way out of the dark
#define	WM_OBJECT_FLAG_destroy		0b0000000100000000	// object to be deleted
#define	WM_OBJECT_FLAG_transparent	0b0000001000000000	// object has transparent zones
#define	WM_OBJECT_FLAG_cursor		0b0000010000000000	// object as a cursor
#define	WM_OBJECT_FLAG_menu		0b0000100000000000	// manage object as pop-up window
#define	WM_OBJECT_FLAG_header		0b0001000000000000	// object contains header
#define	WM_OBJECT_FLAG_lock		0b0010000000000000	// object lock access to desktop
#define WM_OBJECT_FLAG_exist		0b0100000000000000	// object exist

#define	WM_OBJECT_TASKBAR_HEIGHT_pixel	32

#define	WM_TASKBAR_ELEMENT_WIDTH_pixel	(255 + 1)

#define	WM_OBJECT_TASKBAR_ELEMENT_X	WM_OBJECT_TASKBAR_HEIGHT_pixel

struct	WM_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	uint16_t	width;
	uint16_t	height;
	uint32_t	*base_address;
	uint32_t	size_byte;
	uint8_t		id;
	uint16_t	flags;
	uint8_t		length;
	uint8_t		name[ WM_OBJECT_NAME_length ];
	uint64_t	pid;
};

struct	WM_STRUCTURE_ZONE {
	int16_t		x;
	int16_t		y;
	uint16_t	width;
	uint16_t	height;
	uintptr_t	object_address;
};