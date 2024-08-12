/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INTERFACE
	#define	LIB_INTERFACE

	#define	LIB_INTERFACE_NAME_length				38

	#define	LIB_INTERFACE_COLOR_background				0xFF141414
	#define	LIB_INTERFACE_COLOR_foreground				0xFFF5F5F5

	#define	LIB_INTERFACE_HEADER_HEIGHT_pixel			(LIB_FONT_HEIGHT_pixel + 6)	// 3 pixels from above and under
	#define	LIB_INTERFACE_HEADER_COLOR_background			LIB_INTERFACE_COLOR_background

	#define	LIB_INTERFACE_ELEMENT_TYPE_null				0x00
	#define	LIB_INTERFACE_ELEMENT_TYPE_window			0x01
	#define	LIB_INTERFACE_ELEMENT_TYPE_label			0x02
	#define	LIB_INTERFACE_ELEMENT_TYPE_button			0x03
	#define	LIB_INTERFACE_ELEMENT_TYPE_mmc_close			0x04
	#define	LIB_INTERFACE_ELEMENT_TYPE_mmc_minimize			0x05
	#define	LIB_INTERFACE_ELEMENT_TYPE_mmc_maximize			0x06
	#define	LIB_INTERFACE_ELEMENT_TYPE_list				0x07

	#define	LIB_INTERFACE_ELEMENT_FLAG_hover			0b00010000

	#define	LIB_INTERFACE_ELEMENT_BUTTON_padding			4
	#define	LIB_INTERFACE_ELEMENT_BUTTON_COLOR_foreground		0xFFEEEEEE
	#define	LIB_INTERFACE_ELEMENT_BUTTON_COLOR_background		0xFF202020
	#define	LIB_INTERFACE_ELEMENT_BUTTON_COLOR_background_light	0xFF282828
	#define LIB_INTERFACE_ELEMENT_BUTTON_COLOR_border		0xFF242424

	#define	LIB_INTERFACE_ELEMENT_MMC_CLOSE_COLOR_background_hover	0xFF208020

	#define	LIB_INTERFACE_ELEMENT_LIST_COLOR_background0		0xFF222222
	#define	LIB_INTERFACE_ELEMENT_LIST_COLOR_background1		0xFF202020
	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_NAME_length		255
	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_HEIGHT_pixel		22

	#define	LIB_INTERFACE_ELEMENT_MANAGEMENT_size			10

	struct	LIB_INTERFACE_STRUCTURE {
		uintptr_t	properties;
		uint8_t		length;
		uint8_t		name[ LIB_INTERFACE_NAME_length + 1 ];
		uint16_t	width;
		uint16_t	height;
		struct WM_STRUCTURE_DESCRIPTOR	*window_descriptor;
		uint32_t	*base_address;
		uint8_t		id;
		uint8_t		mmc_length;
	};

	struct	LIB_INTERFACE_STRUCTURE_ELEMENT {
		uint8_t		type;
		uint16_t	size_byte;
		void		(*event)( void );
		uint8_t		flags;
		uint16_t	id;
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	mmc;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	label_or_button;
		uint8_t		length;
		uint8_t		name[ 214 ];
		uint8_t		command_length;
		uint8_t		command[ 42 + 1 ];
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	menu;
		uint8_t		length;
		uint8_t		name[ 64 ];
	};

	struct LIB_INTERFACE_STRUCTURE_LIST {
		uint8_t		length;
		uint8_t		name[ LIB_INTERFACE_ELEMENT_LIST_ENTRY_NAME_length ];
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LIST {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	list;
		struct LIB_INTERFACE_STRUCTURE_LIST	*entry;
		uint64_t				entry_count;
	};

	void lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );
	void lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface );
	void lib_interface_clean( struct LIB_INTERFACE_STRUCTURE *interface );
	uint64_t lib_interface_cohesion( uint8_t *interface_json );
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );
	void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface );
	uintptr_t lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id );
	void lib_interface_hover( struct LIB_INTERFACE_STRUCTURE *interface );
	uint64_t lib_interface_length( uint8_t *interface_json );
	void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface );
	void lib_interface_window( struct LIB_INTERFACE_STRUCTURE *interface );
	void lib_interface_element_mmc( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MMC *element );
	void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );
	void lib_interface_element_button( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );
#endif