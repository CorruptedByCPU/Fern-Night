/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void taris_init() {
	// initialize interface of application
	lib_interface( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface );

	// find control element for close function
	struct LIB_INTERFACE_STRUCTURE_ELEMENT *taris_close = (struct LIB_INTERFACE_STRUCTURE_ELEMENT *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 1 ); taris_close -> event = (void *) close;

	// find label element with points value
	taris_points = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 2 );

	// find label element with lines value
	taris_lines = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 3 );

	// find label element with level value
	taris_level = (struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *) lib_interface_element_by_id( (struct LIB_INTERFACE_STRUCTURE *) &taris_interface, 4 );

	// initialize Raw Graphics Layer
	taris_rgl.base_address = taris_interface.base_address + (TARIS_WINDOW_WIDTH_pixel * (LIB_INTERFACE_HEADER_HEIGHT_pixel + TARIS_MENU_HEIGHT_pixel)) + 1;
	lib_rgl( (struct LIB_RGL_STRUCTURE *) &taris_rgl );

	// clean up playground
	lib_rgl_clean( (struct LIB_RGL_STRUCTURE *) &taris_rgl );
}