/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	DRIVER_PS2
	#define	DRIVER_PS2

	// external routines (assembly language)
	extern void driver_ps2_mouse_entry( void );
	extern void driver_ps2_keyboard_entry( void );

	#define	DRIVER_PS2_KEYBOARD_CACHE_limit			7

	#define	DRIVER_PS2_KEYBOARD_IRQ_number			0x01
	#define	DRIVER_PS2_KEYBOARD_IO_APIC_register		KERNEL_IO_APIC_iowin + (DRIVER_PS2_KEYBOARD_IRQ_number * 0x02)

	#define	DRIVER_PS2_MOUSE_IRQ_number			0x0C
	#define	DRIVER_PS2_MOUSE_IO_APIC_register		KERNEL_IO_APIC_iowin + (DRIVER_PS2_MOUSE_IRQ_number * 0x02)

	#define	DRIVER_PS2_PORT_DATA				0x0060
	#define	DRIVER_PS2_PORT_COMMAND_OR_STATUS		0x0064

	#define	DRIVER_PS2_COMMAND_CONFIGURATION_GET		0x20
	#define	DRIVER_PS2_COMMAND_CONFIGURATION_SET		0x60
	#define	DRIVER_PS2_COMMAND_PORT_SECOND			0xD4

	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_LMB		0b00000001
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_RMB		0b00000010
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_MMB		0b00000100
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_ALWAYS_ONE	0b00001000
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_X_SIGNED		0b00010000
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_Y_SIGNED		0b00100000
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_OVERFLOW_x	0b01000000
	#define	DRIVER_PS2_DEVICE_MOUSE_PACKET_OVERFLOW_y	0b10000000

	#define	DRIVER_PS2_DEVICE_PACKETS_ENABLE		0xF4
	#define	DRIVER_PS2_DEVICE_SET_DEFAULT			0xF6
	#define	DRIVER_PS2_DEVICE_RESET				0xFF

	#define	DRIVER_PS2_ANSWER_ACKNOWLEDGED			0xFA
	#define	DRIVER_PS2_ANSWER_SELF_TEST_SUCCESS		0xAA

	#define	DRIVER_PS2_STATUS_output			0b00000001
	#define	DRIVER_PS2_STATUS_input				0b00000010
	#define	DRIVER_PS2_STATUS_output_second			0b00100000

	#define	DRIVER_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT	0b00000010
	#define	DRIVER_PS2_CONFIGURATION_PORT_SECOND_CLOCK	0b00100000

	#define	DRIVER_PS2_KEYBOARD_sequence			0xE0
	#define	DRIVER_PS2_KEYBOARD_sequence_alternative	0xE1

	#define	DRIVER_PS2_KEYBOARD_PRESS_BACKSPACE		0x0008
	#define	DRIVER_PS2_KEYBOARD_PRESS_TAB			0x0009
	#define	DRIVER_PS2_KEYBOARD_PRESS_ENTER			0x000D
	#define	DRIVER_PS2_KEYBOARD_PRESS_ESC			0x001B
	#define	DRIVER_PS2_KEYBOARD_PRESS_CTRL_LEFT		0x001D
	#define	DRIVER_PS2_KEYBOARD_PRESS_SPACE			0x0020
	#define	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_LEFT		0xE02A
	#define	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_RIGHT		0xE036
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MULTIPLY	0xE037
	#define	DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT		0xE038
	#define	DRIVER_PS2_KEYBOARD_PRESS_CAPSLOCK		0xE03A
	#define	DRIVER_PS2_KEYBOARD_PRESS_F1			0xE03B
	#define	DRIVER_PS2_KEYBOARD_PRESS_F2			0xE03C
	#define	DRIVER_PS2_KEYBOARD_PRESS_F3			0xE03D
	#define	DRIVER_PS2_KEYBOARD_PRESS_F4			0xE03E
	#define	DRIVER_PS2_KEYBOARD_PRESS_F5			0xE03F
	#define	DRIVER_PS2_KEYBOARD_PRESS_F6			0xE040
	#define	DRIVER_PS2_KEYBOARD_PRESS_F7			0xE041
	#define	DRIVER_PS2_KEYBOARD_PRESS_F8			0xE042
	#define	DRIVER_PS2_KEYBOARD_PRESS_F9			0xE043
	#define	DRIVER_PS2_KEYBOARD_PRESS_F10			0xE044
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK		0xE045
	#define	DRIVER_PS2_KEYBOARD_PRESS_SCROLL_LOCK		0xE046
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MINUS		0xE04A
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_4		0xE14B
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_5		0xE04C
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_6		0xE14D
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_PLUS		0xE04E
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_7		0xE047
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_8		0xE048
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_9		0xE049
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_1		0xE14F
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_2		0xE150
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_3		0xE151
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_0		0xE152
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DOT		0xE153
	#define	DRIVER_PS2_KEYBOARD_PRESS_F11			0xE057
	#define	DRIVER_PS2_KEYBOARD_PRESS_F12			0xE158
	#define	DRIVER_PS2_KEYBOARD_PRESS_CTRL_RIGHT		0xE01D
	#define	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DIVIDE	0xE035
	#define	DRIVER_PS2_KEYBOARD_PRESS_ALT_RIGHT		0xE038
	#define	DRIVER_PS2_KEYBOARD_PRESS_HOME			0xE047
	#define	DRIVER_PS2_KEYBOARD_PRESS_ARROW_UP		0xE048
	#define	DRIVER_PS2_KEYBOARD_PRESS_PAGE_UP		0xE049
	#define	DRIVER_PS2_KEYBOARD_PRESS_ARROW_LEFT		0xE04B
	#define	DRIVER_PS2_KEYBOARD_PRESS_ARROW_RIGHT		0xE04D
	#define	DRIVER_PS2_KEYBOARD_PRESS_END			0xE04F
	#define	DRIVER_PS2_KEYBOARD_PRESS_ARROW_DOWN		0xE050
	#define	DRIVER_PS2_KEYBOARD_PRESS_PAGE_DOWN		0xE051
	#define	DRIVER_PS2_KEYBOARD_PRESS_INSERT		0xE052
	#define	DRIVER_PS2_KEYBOARD_PRESS_DELETE		0xE053
	#define	DRIVER_PS2_KEYBOARD_PRESS_WIN_LEFT		0xE058
	#define	DRIVER_PS2_KEYBOARD_PRESS_MOUSE_RIGHT		0xE05D

	#define	DRIVER_PS2_KEYBOARD_key_release			0x0080

	#define	DRIVER_PS2_KEYBOARD_RELEASE_BACKSPACE		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_BACKSPACE
	#define	DRIVER_PS2_KEYBOARD_RELEASE_TAB			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_TAB
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ENTER		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ENTER
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ESC			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ESC
	#define	DRIVER_PS2_KEYBOARD_RELEASE_CTRL_LEFT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_CTRL_LEFT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_SHIFT_LEFT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_SHIFT_LEFT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_SHIFT_RIGHT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_SHIFT_RIGHT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_MULTIPLY	DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MULTIPLY
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ALT_LEFT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_CAPSLOCK		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_CAPSLOCK
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F1			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F1
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F2			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F2
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F3			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F3
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F4			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F4
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F5			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F5
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F6			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F6
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F7			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F7
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F8			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F8
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F9			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F9
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F10			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F10
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK
	#define	DRIVER_PS2_KEYBOARD_RELEASE_SCROLL_LOCK		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_SCROLL_LOCK
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_MINUS	DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MINUS
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_4		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_4
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_5		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_5
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_6		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_6
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_PLUS	DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_PLUS
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_1		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_1
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_2		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_2
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_3		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_3
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_0		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_0
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_DOT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DOT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F11			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F11
	#define	DRIVER_PS2_KEYBOARD_RELEASE_F12			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_F12
	#define	DRIVER_PS2_KEYBOARD_RELEASE_CTRL_RIGHT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_CTRL_RIGHT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_NUMLOCK_DIVIDE	DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DIVIDE
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ALT_RIGHT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ALT_RIGHT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_HOME		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_HOME
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ARROW_UP		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ARROW_UP
	#define	DRIVER_PS2_KEYBOARD_RELEASE_PAGE_UP		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_PAGE_UP
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ARROW_LEFT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ARROW_LEFT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ARROW_RIGHT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ARROW_RIGHT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_END			DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_END
	#define	DRIVER_PS2_KEYBOARD_RELEASE_ARROW_DOWN		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_ARROW_DOWN
	#define	DRIVER_PS2_KEYBOARD_RELEASE_PAGE_DOWN		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_PAGE_DOWN
	#define	DRIVER_PS2_KEYBOARD_RELEASE_INSERT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_INSERT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_DELETE		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_DELETE
	#define	DRIVER_PS2_KEYBOARD_RELEASE_WIN_LEFT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_WIN_LEFT
	#define	DRIVER_PS2_KEYBOARD_RELEASE_MOUSE_RIGHT		DRIVER_PS2_KEYBOARD_key_release + DRIVER_PS2_KEYBOARD_PRESS_MOUSE_RIGHT

	void driver_ps2_init();
	void driver_ps2_drain();
	void driver_ps2_check_read();
	void driver_ps2_check_write();
	void driver_ps2_command( uint8_t command );
	void driver_ps2_data_write( uint8_t data );
	uint8_t driver_ps2_data_read();
	__attribute__(( preserve_most )) void driver_ps2_mouse( void );
	__attribute__(( preserve_most )) void driver_ps2_keyboard( void );
	void driver_ps2_keyboard_key_save( uint16_t key );
	uint16_t driver_ps2_keyboard_key_read();
#endif