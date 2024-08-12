/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init() {
	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) lib_sys_memory_alloc( WM_OBJECT_limit * sizeof( struct WM_STRUCTURE_OBJECT ) );

	// prepare space for an array of object list
	wm_list_base_address = (uintptr_t *) lib_sys_memory_alloc( WM_LIST_limit << STATIC_MULTIPLE_BY_PTR_shift );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) lib_sys_memory_alloc( WM_ZONE_limit * sizeof( struct WM_STRUCTURE_ZONE ) );

	// prepare space for an list of zones
	wm_taskbar_elements = (uintptr_t *) lib_sys_memory_alloc( WM_OBJECT_limit * sizeof( struct WM_STRUCTURE_OBJECT ) );

	// get our PID number
	wm_pid_number = lib_sys_task_pid();

	// request access and properties of Framebuffer
	lib_sys_framebuffer( (struct LIB_SYS_STRUCTURE_FRAMEBUFFER *) &wm_framebuffer );

	// framebuffer locked?
	if( wm_framebuffer.pid != wm_pid_number ) { printf( "Framebuffer is already in use. Exit." ); lib_sys_exit(); }

	// set cache properties
	wm_object_framebuffer.width = wm_framebuffer.width_pixel;
	wm_object_framebuffer.height = wm_framebuffer.height_pixel;
	wm_object_framebuffer.size_byte = wm_framebuffer.scanline_byte * wm_framebuffer.height_pixel;
	wm_object_framebuffer.base_address = (uint32_t *) lib_sys_memory_alloc( wm_object_framebuffer.size_byte + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

	//----------------------------------------------------------------------

	// create workbench window
	wm_object_workbench = (struct WM_STRUCTURE_OBJECT *) wm_object_create( 0, 0, wm_object_framebuffer.width, wm_object_framebuffer.height, WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_fixed_xy );
	wm_object_workbench -> base_address = (uint32_t *) lib_sys_memory_alloc( wm_object_framebuffer.size_byte + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

	// load image file of cursor
	lib_sys_storage_read_file( &file_wallpaper );

	// file exist?
	if( file_wallpaper.size_byte ) {
		// get properties of image
		struct LIB_IMAGE_TGA_STRUCTURE *image_wallpaper = (struct LIB_IMAGE_TGA_STRUCTURE *) file_wallpaper.address;

		// load the image into the window space
		uint32_t *tmp = (uint32_t *) lib_sys_memory_alloc( image_wallpaper -> width * image_wallpaper -> height * 4 );
		lib_image_tga_parse( (uint8_t *) file_wallpaper.address, tmp, file_wallpaper.size_byte );

		// scale loaded image to workbench properties
		float x_scale_factor = (float) ((float) image_wallpaper -> width / (float) wm_object_workbench -> width);
		float y_scale_factor = (float) ((float) image_wallpaper -> height / (float) wm_object_workbench -> height);
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				wm_object_workbench -> base_address[ (y * wm_object_workbench -> width) + x ] = tmp[ (uint64_t) (((uint64_t) (y_scale_factor * y) * image_wallpaper -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary space
		lib_sys_memory_release( (uintptr_t) tmp, image_wallpaper -> width * image_wallpaper -> height * 4 );

		// blur image for nice view
		// lib_image_blur( wm_object_workbench -> base_address, 3, wm_object_workbench -> width, wm_object_workbench -> height );

		// release file
		lib_sys_memory_release( file_wallpaper.address, file_wallpaper.size_byte );
	} else
		// fill workbench window with default color
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				wm_object_workbench -> base_address[ (y * wm_object_workbench -> width) + x ] = 0xFF000000;

	// show debug information
	uint8_t build_version[] = "System build on "__DATE__" "__TIME__;
	lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, build_version, sizeof( build_version ) - 1, STATIC_COLOR_gray, (uint32_t *) (wm_object_workbench -> base_address), wm_object_workbench -> width, LIB_FONT_ALIGN_right );

	// set as our object
	wm_object_workbench -> pid = wm_pid_number;

	// refresh content of object in framebuffer
	wm_object_workbench -> flags |= WM_OBJECT_FLAG_flush;

	//----------------------------------------------------------------------

	// create taskbar object
	wm_object_taskbar = (struct WM_STRUCTURE_OBJECT *) wm_object_create( EMPTY, wm_framebuffer.height_pixel - WM_OBJECT_TASKBAR_HEIGHT_pixel, wm_framebuffer.width_pixel, WM_OBJECT_TASKBAR_HEIGHT_pixel, WM_OBJECT_FLAG_arbiter | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_fixed_xy );
	wm_object_taskbar -> base_address = (uint32_t *) lib_sys_memory_alloc( wm_object_taskbar -> size_byte + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

	//----------------------------------------------------------------------

	// load image file of cursor
	lib_sys_storage_read_file( &file_cursor );

	// file exist?
	if( file_cursor.size_byte ) {
		// get properties of image
		struct LIB_IMAGE_TGA_STRUCTURE *image_cursor = (struct LIB_IMAGE_TGA_STRUCTURE *) file_cursor.address;

		// create cursor window
		wm_object_cursor = (struct WM_STRUCTURE_OBJECT *) wm_object_create( wm_object_framebuffer.width >> STATIC_DIVIDE_BY_2_shift, wm_object_framebuffer.height >> STATIC_DIVIDE_BY_2_shift, image_cursor -> width, image_cursor -> height, WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_cursor );
		wm_object_cursor -> base_address = (uint32_t *) lib_sys_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_cursor -> size_byte ) + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

		// load the image into the window space
		lib_image_tga_parse( (uint8_t *) file_cursor.address, wm_object_cursor -> base_address, file_cursor.size_byte );

		// refresh content of object in framebuffer
		wm_object_cursor -> flags |= WM_OBJECT_FLAG_flush;
	} else {
		// create cursor window
		wm_object_cursor = (struct WM_STRUCTURE_OBJECT *) wm_object_create( wm_object_framebuffer.width >> STATIC_DIVIDE_BY_2_shift, wm_object_framebuffer.height >> STATIC_DIVIDE_BY_2_shift, 16, 16, WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_cursor );
		wm_object_cursor -> base_address = (uint32_t *) lib_sys_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_cursor -> size_byte ) + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

		// fill workbench window with default color
		for( uint16_t y = 0; y < wm_object_cursor -> height; y++ )
			for( uint16_t x = 0; x < wm_object_cursor -> width; x++ )
				wm_object_cursor -> base_address[ (y * wm_object_cursor -> width) + x ] = 0xFFFFFFFF;
	}

	// set as our object
	wm_object_cursor -> pid = wm_pid_number;

	//----------------------------------------------------------------------

	// enable menu thread
	uint8_t wm_string_thread_menu[] = "menu";
	lib_sys_thread( (uintptr_t) &wm_thread_menu, (uint8_t *) &wm_string_thread_menu, sizeof( wm_string_thread_menu ) - 1 );

	// enable taskbar thread
	uint8_t wm_string_thread_taskbar[] = "taskbar";
	lib_sys_thread( (uintptr_t) &wm_thread_taskbar, (uint8_t *) &wm_string_thread_taskbar, sizeof( wm_string_thread_taskbar ) - 1 );

	// execute access to terminal
	// lib_sys_exec( (uint8_t *) &wm_exec_console, sizeof( wm_exec_console ) - 1, LIB_SYS_STREAM_FLOW_out_to_parent_out );
}
