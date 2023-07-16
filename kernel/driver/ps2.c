/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

uint8_t		driver_ps2_mouse_semaphore		= FALSE;
uint8_t		driver_ps2_mouse_type			= EMPTY;
uint8_t		driver_ps2_mouse_packet			= EMPTY;

uint16_t	driver_ps2_keyboard_storage[ DRIVER_PS2_KEYBOARD_CACHE_limit + 1 ] 	= { EMPTY };
uint8_t		driver_ps2_keyboard_matrix		= FALSE;

uint16_t	driver_ps2_scancode 			= EMPTY;

uint16_t	driver_ps2_keyboard_matrix_low[] = {
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_ESC,	// Escape
	0x0031,	// 1
	0x0032,	// 2
	0x0033,	// 3
	0x0034,	// 4
	0x0035,	// 5
	0x0036,	// 6
	0x0037,	// 7
	0x0038,	// 8
	0x0039,	// 9
	0x0030,	// 0
	0x002D,	// -
	0x003D,	// =
	DRIVER_PS2_KEYBOARD_PRESS_BACKSPACE,
	DRIVER_PS2_KEYBOARD_PRESS_TAB,
	0x0071,	// q
	0x0077,	// w
	0x0065,	// e
	0x0072,	// r
	0x0074,	// t
	0x0079,	// y
	0x0075,	// u
	0x0069,	// i
	0x006F,	// o
	0x0070,	// p
	0x005B,	// [
	0x005D,	// ]
	DRIVER_PS2_KEYBOARD_PRESS_ENTER,
	DRIVER_PS2_KEYBOARD_PRESS_CTRL_LEFT,
	0x0061,	// a
	0x0073,	// s
	0x0064,	// d
	0x0066,	// f
	0x0067,	// g
	0x0068,	// h
	0x006A,	// j
	0x006B,	// k
	0x006C,	// l
	0x003B,	// ;
	0x0027,	// '
	0x0060,	// `
	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_LEFT,
	0x005C,	// \\
	0x0000,
	0x007A,	// z
	0x0078,	// x
	0x0063,	// c
	0x0076,	// v
	0x0062,	// b
	0x006E,	// n
	0x006D,	// m
	0x002C,	// ,
	0x002E,	// .
	0x002F,	// /
	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_RIGHT,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MULTIPLY,
	DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT,
	DRIVER_PS2_KEYBOARD_PRESS_SPACE,
	DRIVER_PS2_KEYBOARD_PRESS_CAPSLOCK,
	DRIVER_PS2_KEYBOARD_PRESS_F1,
	DRIVER_PS2_KEYBOARD_PRESS_F2,
	DRIVER_PS2_KEYBOARD_PRESS_F3,
	DRIVER_PS2_KEYBOARD_PRESS_F4,
	DRIVER_PS2_KEYBOARD_PRESS_F5,
	DRIVER_PS2_KEYBOARD_PRESS_F6,
	DRIVER_PS2_KEYBOARD_PRESS_F7,
	DRIVER_PS2_KEYBOARD_PRESS_F8,
	DRIVER_PS2_KEYBOARD_PRESS_F9,
	DRIVER_PS2_KEYBOARD_PRESS_F10,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK,
	DRIVER_PS2_KEYBOARD_PRESS_SCROLL_LOCK,
	EMPTY,
	EMPTY,
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_7,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_8,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_9,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MINUS,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_4,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_5,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_6,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_PLUS,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_1,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_2,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_3,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_0,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DOT,
	EMPTY,
	EMPTY,
	EMPTY,
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_F11,
	DRIVER_PS2_KEYBOARD_PRESS_F12
};

uint16_t driver_ps2_keyboard_matrix_high[] = {
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_ESC,
	0x0021,	// !
	0x0040,	// @
	0x0023,	// #
	0x0024,	// $
	0x0025,	// %
	0x005E,	// ^
	0x0026,	// &
	0x002A,	// *
	0x0028,	// ()
	0x0029,	// )
	0x005F,	// _
	0x002B,	// +
	DRIVER_PS2_KEYBOARD_PRESS_BACKSPACE,
	DRIVER_PS2_KEYBOARD_PRESS_TAB,
	0x0051,	// Q
	0x0057,	// W
	0x0045,	// E
	0x0052,	// R
	0x0054,	// T
	0x0059,	// Y
	0x0055,	// U
	0x0049,	// I
	0x004F,	// O
	0x0050,	// P
	0x007B,	// }
	0x007D,	// {
	DRIVER_PS2_KEYBOARD_PRESS_ENTER,
	DRIVER_PS2_KEYBOARD_PRESS_CTRL_LEFT,
	0x0041,	// A
	0x0053,	// S
	0x0044,	// D
	0x0046,	// F
	0x0047,	// G
	0x0048,	// H
	0x004A,	// J
	0x004B,	// K
	0x004C,	// L
	0x003A,	// :
	0x0022,	// "
	0x007E,	// ~
	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_LEFT,
	0x007C,	// |
	0x005A,	// Z
	0x0058,	// X
	0x0043,	// C
	0x0056,	// V
	0x0042,	// B
	0x004E,	// N
	0x004D,	// M
	0x003C,	// <
	0x003E,	// >
	0x003F,	// ?
	DRIVER_PS2_KEYBOARD_PRESS_SHIFT_RIGHT,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MULTIPLY,
	DRIVER_PS2_KEYBOARD_PRESS_ALT_LEFT,
	DRIVER_PS2_KEYBOARD_PRESS_SPACE,
	DRIVER_PS2_KEYBOARD_PRESS_CAPSLOCK,
	DRIVER_PS2_KEYBOARD_PRESS_F1,
	DRIVER_PS2_KEYBOARD_PRESS_F2,
	DRIVER_PS2_KEYBOARD_PRESS_F3,
	DRIVER_PS2_KEYBOARD_PRESS_F4,
	DRIVER_PS2_KEYBOARD_PRESS_F5,
	DRIVER_PS2_KEYBOARD_PRESS_F6,
	DRIVER_PS2_KEYBOARD_PRESS_F7,
	DRIVER_PS2_KEYBOARD_PRESS_F8,
	DRIVER_PS2_KEYBOARD_PRESS_F9,
	DRIVER_PS2_KEYBOARD_PRESS_F10,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK,
	DRIVER_PS2_KEYBOARD_PRESS_SCROLL_LOCK,
	EMPTY,
	EMPTY,
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_7,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_8,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_9,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_MINUS,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_4,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_5,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_6,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_PLUS,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_1,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_2,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_3,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_0,
	DRIVER_PS2_KEYBOARD_PRESS_NUMLOCK_DOT,
	EMPTY,
	EMPTY,
	EMPTY,
	EMPTY,
	DRIVER_PS2_KEYBOARD_PRESS_F11,
	DRIVER_PS2_KEYBOARD_PRESS_F12
};

void driver_ps2_check_read() {
	// wait for output data
	while( ! (driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_output) );
}

void driver_ps2_check_write() {
	// wait for input data
	while( driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_input );
}

void driver_ps2_command( uint8_t command ) {
	// wait for controller to be ready to accept command
	driver_ps2_check_write();

	// send command
	driver_port_out_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS, command );
}

uint8_t driver_ps2_data_read() {
	// wait for controller to be ready to accept answer/data
	driver_ps2_check_read();

	// receive answer/data
	return driver_port_in_byte( DRIVER_PS2_PORT_DATA );
}

void driver_ps2_data_write( uint8_t data ) {
	// wait for controller to be ready to accept command/data
	driver_ps2_check_write();

	// send data
	driver_port_out_byte( DRIVER_PS2_PORT_DATA, data );
}

void driver_ps2_drain() {
	// flush PS2 controller output buffer
	while( driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_output )
		// drop data from PS2 controller
		driver_port_in_byte( DRIVER_PS2_PORT_DATA );
	
	// there is nothig left, good
}

__attribute__(( no_caller_saved_registers ))
void driver_ps2_mouse( void ) {
	// data from second controller port?
	if( driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_output_second ) {
		// get package from PS2 controller
		int8_t packet = driver_ps2_data_read();

		// perform operation depending on number of package
		switch( driver_ps2_mouse_packet ) {
			case 0:
				// status packet contains ALWAYS ON bit?
				if( ! (packet & DRIVER_PS2_DEVICE_MOUSE_PACKET_ALWAYS_ONE ) ) break;	// no

				// overflow on X axis?
				if( packet & DRIVER_PS2_DEVICE_MOUSE_PACKET_OVERFLOW_x ) break;	// yes

				// overflow on Y axis?
				if( packet & DRIVER_PS2_DEVICE_MOUSE_PACKET_OVERFLOW_y ) break;	// yes

				// save device status: mouse
				kernel -> driver_ps2_mouse_status = packet;

				// package handled from given interrupt
				driver_ps2_mouse_packet++;
				break;
			case 1:
				// value on X axis negative?
				if( kernel -> driver_ps2_mouse_status & DRIVER_PS2_DEVICE_MOUSE_PACKET_X_SIGNED ) {
					packet = ~packet + 1;

					// keep offset
					if( kernel -> driver_ps2_mouse_x - packet < 0 ) kernel -> driver_ps2_mouse_x = EMPTY;
					else kernel -> driver_ps2_mouse_x -= packet;

					// calculate relative position
					kernel -> driver_ps2_mouse_x_absolute -= packet;
				} else {
					// keep offset
					if( kernel -> driver_ps2_mouse_x + packet < kernel -> framebuffer_width_pixel ) kernel -> driver_ps2_mouse_x += packet;
					else kernel -> driver_ps2_mouse_x = kernel -> framebuffer_width_pixel - 1;

					// calculate relative position
					kernel -> driver_ps2_mouse_x_absolute += packet;
				}

				// package handled from given interrupt
				driver_ps2_mouse_packet++;
				break;
			case 2:
				// value on Y axis negative?
				if( kernel -> driver_ps2_mouse_status & DRIVER_PS2_DEVICE_MOUSE_PACKET_Y_SIGNED ) {
					packet = ~packet + 1;

					// keep offset
					if( kernel -> driver_ps2_mouse_y + packet < kernel -> framebuffer_height_pixel ) kernel -> driver_ps2_mouse_y += packet;
					else kernel -> driver_ps2_mouse_y = kernel -> framebuffer_height_pixel - 1;

					// calculate relative position
					kernel -> driver_ps2_mouse_y_absolute += packet;
				} else {
					// keep offset
					if( kernel -> driver_ps2_mouse_y - packet < 0 ) kernel -> driver_ps2_mouse_y = EMPTY;
					else kernel -> driver_ps2_mouse_y -= packet;

					// calculate relative position
					kernel -> driver_ps2_mouse_y_absolute -= packet;
				}

				// package handled from given interrupt
				driver_ps2_mouse_packet = EMPTY;
				break;
		}
	}

	// tell LAPIC of current logical processor that hardware interrupt is being handled, properly
	kernel_lapic_accept();
}

uint16_t driver_ps2_keyboard_key_read() {
	// Am I framebuffer manager?
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();
	if( task -> pid != kernel -> framebuffer_manager ) return EMPTY;	// no

	// get first key code in buffer
	uint16_t key = driver_ps2_keyboard_storage[ 0 ];

	// move all characters in buffer forward one position
	for( uint8_t i = 0; i < DRIVER_PS2_KEYBOARD_CACHE_limit; i++ )
		driver_ps2_keyboard_storage[ i ] = driver_ps2_keyboard_storage[ i + 1 ];

	// return a key code
	return key;
}

__attribute__(( no_caller_saved_registers ))
void driver_ps2_keyboard( void ) {
	// get key code
	uint16_t key = driver_ps2_data_read();

	// perform operation depending on opcode
	switch( key ) {
		// controller started sequence?
		case DRIVER_PS2_KEYBOARD_sequence:
			// save sequence type
			driver_ps2_scancode = key << STATIC_MOVE_AL_TO_HIGH_shift;
			break;

		// controller started alternate sequence?
		case DRIVER_PS2_KEYBOARD_sequence_alternative:
			// zachowaj typ sekwencji
			driver_ps2_scancode = key << STATIC_MOVE_AL_TO_HIGH_shift;
			break;

		// controller doesn't start sequence?
		default:
			// complete sequence?
			if( driver_ps2_scancode ) {
				// compose key code
				key |= driver_ps2_scancode;

				// sequence processed
				driver_ps2_scancode = EMPTY;
			} else {
				// key code not in matrix?
				if( key >= DRIVER_PS2_KEYBOARD_key_release ) {
					// get ASCII code for key from matrix
					if( ! driver_ps2_keyboard_matrix ) key = driver_ps2_keyboard_matrix_low[ key - DRIVER_PS2_KEYBOARD_key_release ];
					else key = driver_ps2_keyboard_matrix_high[ key - DRIVER_PS2_KEYBOARD_key_release];

					// correct key code
					key += DRIVER_PS2_KEYBOARD_key_release;
				} else
					// get ASCII code for key from matrix
					if( ! driver_ps2_keyboard_matrix ) key = driver_ps2_keyboard_matrix_low[ key ];
					else key = driver_ps2_keyboard_matrix_high[ key ];
			}

			// SHIFT or CAPSLOCK key?
			if( key == DRIVER_PS2_KEYBOARD_PRESS_CAPSLOCK || key == DRIVER_PS2_KEYBOARD_PRESS_SHIFT_LEFT || key == DRIVER_PS2_KEYBOARD_PRESS_SHIFT_RIGHT ) {
				if( driver_ps2_keyboard_matrix ) driver_ps2_keyboard_matrix = FALSE;
				else driver_ps2_keyboard_matrix = TRUE;
			} else if( key == DRIVER_PS2_KEYBOARD_RELEASE_SHIFT_LEFT || key == DRIVER_PS2_KEYBOARD_RELEASE_SHIFT_RIGHT ) {
				if( driver_ps2_keyboard_matrix ) driver_ps2_keyboard_matrix = FALSE;
				else driver_ps2_keyboard_matrix = TRUE;
			}

			// in first free space in keyboard buffer
			for( uint8_t i = 0; i < DRIVER_PS2_KEYBOARD_CACHE_limit; i++ )
				// save key code
				if( ! driver_ps2_keyboard_storage[ i ] ) { driver_ps2_keyboard_storage[ i ] = key; break; }
	}

	// tell LAPIC of current logical processor that hardware interrupt is being handled, propely
	kernel_lapic_accept();
}

void driver_ps2_init() {
	// drain PS2 controller buffer
	driver_ps2_drain();

	// retrieve PS2 controller configuration
	driver_ps2_command( DRIVER_PS2_COMMAND_CONFIGURATION_GET );
	uint8_t configuration = driver_ps2_data_read();

	// turn on interrupts and clock on device: mouse
	driver_ps2_command( DRIVER_PS2_COMMAND_CONFIGURATION_SET );
	driver_ps2_data_write( (configuration | DRIVER_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT) & ~DRIVER_PS2_CONFIGURATION_PORT_SECOND_CLOCK );

	// send a RESET command to device: mouse
	driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
	driver_ps2_data_write( DRIVER_PS2_DEVICE_RESET );

	// command accepted?
	if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
		// device is working properly?
		if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_SELF_TEST_SUCCESS ) {
			// get device ID
			driver_ps2_mouse_type = driver_ps2_data_read();

			// send SET DEFAULT command to device: mouse
			driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
			driver_ps2_data_write( DRIVER_PS2_DEVICE_SET_DEFAULT );

			// command accepted?
			if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
				// send PACKETS ENABLE command to device: mouse
				driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
				driver_ps2_data_write( DRIVER_PS2_DEVICE_PACKETS_ENABLE );

				// command accepted?
				if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
					// connect PS2 controller interrupt handler for device: mouse
					kernel_idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_PS2_MOUSE_IRQ_number, KERNEL_IDT_TYPE_irq, (uint64_t) driver_ps2_mouse_entry );

					// connect interrupt vector from IDT table in IOAPIC controller
					kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_PS2_MOUSE_IRQ_number, DRIVER_PS2_MOUSE_IO_APIC_register );
				}
			}
		}
	}

	// connect PS2 controller interrupt handler for device: keyboard
	kernel_idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_PS2_KEYBOARD_IRQ_number, KERNEL_IDT_TYPE_irq, (uint64_t) driver_ps2_keyboard_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_PS2_KEYBOARD_IRQ_number, DRIVER_PS2_KEYBOARD_IO_APIC_register );

	// set initial position of the cursor pointer
	kernel -> driver_ps2_mouse_x = kernel -> framebuffer_width_pixel >> STATIC_DIVIDE_BY_2_shift;
	kernel -> driver_ps2_mouse_y = kernel -> framebuffer_height_pixel >> STATIC_DIVIDE_BY_2_shift;
}
