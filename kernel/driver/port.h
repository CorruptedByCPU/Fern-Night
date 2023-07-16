/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef DRIVER_PORT
	#define	DRIVER_PORT

	inline void driver_port_out_byte( uint16_t port, uint8_t value );
	inline void driver_port_out_dword( uint16_t port, uint32_t value );
	inline uint8_t driver_port_in_byte( uint16_t port );
	inline uint32_t driver_port_in_dword( uint16_t port );
#endif
