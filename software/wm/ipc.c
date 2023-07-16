/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void wm_ipc_keyboard( uint16_t key ) {
	// ignore if there is now active object
	if( ! wm_object_active ) return;

	// prepare the message
	struct LIB_SYS_STRUCTURE_IPC_KEYBOARD message;
	message.type = LIB_SYS_IPC_TYPE_KEYBOARD;

	// and the key itself
	message.key = key;

	// send to active object
	lib_sys_ipc_send( wm_object_active -> pid, (uint8_t *) &message );
}

void wm_ipc_mouse( uint8_t status ) {
	// ignore if there is now active object
	if( ! wm_object_selected ) return;

	// prepare the message
	struct LIB_SYS_STRUCTURE_IPC_MOUSE message;
	message.type = LIB_SYS_IPC_TYPE_MOUSE;

	// and mouse status itself
	message.status = status;

	// send the message to the owner of the object
	lib_sys_ipc_send( wm_object_selected -> pid, (uint8_t *) &message );
}