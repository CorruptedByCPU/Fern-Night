/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	DAEMON

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	// global --------------------------------------------------------------
	#include	"../default.h"
	// library -------------------------------------------------------------
	#include	"../library/shared/elf.h"
	#include	"../library/vfs.h"
	#include	"../library/shared/string.h"
	#include	"../library/shared/sys.h"
	// kernel --------------------------------------------------------------
	#include	"config.h"
	#include	"exec.h"
	// #include	"hpet.h"
	#include	"idt.h"
	#include	"io_apic.h"
	#include	"ipc.h"
	#include	"lapic.h"
	#include	"library.h"
	#include	"log.h"
	#include	"memory.h"
	#include	"page.h"
	#include	"service.h"
	#include	"storage.h"
	#include	"stream.h"
	#include	"task.h"
	// drivers -------------------------------------------------------------
	#include	"driver/port.h"
	#include	"driver/ps2.h"
	#include	"driver/rtc.h"
	#include	"driver/serial.h"
	// kernel environment initialization routines ----------------------------
	#include	"init/acpi.h"
	#include	"init/ap.h"
	#include	"init/cmd.h"
	#include	"init/daemon.h"
	#include	"init/env.h"
	#include	"init/exec.h"
	#include	"init/free.h"
	#include	"init/gdt.h"
	#include	"init/idt.h"
	#include	"init/ipc.h"
	#include	"init/memory.h"
	#include	"init/page.h"
	#include	"init/smp.h"
	#include	"init/storage.h"
	#include	"init/stream.h"
	#include	"init/task.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, functions, routines
	//----------------------------------------------------------------------
	// limine structures/arrays --------------------------------------------
	#include	"init/limine.c"
	// library -------------------------------------------------------------
	#include	"../library/shared/elf.c"
	#include	"../library/vfs.c"
	#include	"../library/shared/string.c"
	// kernel --------------------------------------------------------------
	#include	"data.c"
	#include	"exec.c"
	// #include	"hpet.c"
	#include	"idt.c"
	#include	"io_apic.c"
	#include	"lapic.c"
	#include	"library.c"
	#include	"log.c"
	#include	"memory.c"
	#include	"page.c"
	#include	"rtc.c"
	#include	"service.c"
	#include	"storage.c"
	#include	"stream.c"
	#include	"task.c"
	// drivers -------------------------------------------------------------
	#include	"driver/port.c"
	#include	"driver/ps2.c"
	#include	"driver/rtc.c"
	#include	"driver/serial.c"
	// kernel environment initialization routines --------------------------
	#include	"init/acpi.c"
	#include	"init/ap.c"
	#include	"init/data.c"
	#include	"init/cmd.c"
	#include	"init/daemon.c"
	#include	"init/env.c"
	#include	"init/exec.c"
	#include	"init/free.c"
	#include	"init/gdt.c"
	// #include	"init/hpet.c"
	#include	"init/idt.c"
	#include	"init/ipc.c"
	#include	"init/library.c"
	#include	"init/memory.c"
	#include	"init/page.c"
	#include	"init/rtc.c"
	#include	"init/smp.c"
	#include	"init/storage.c"
	#include	"init/stream.c"
	#include	"init/task.c"
	//======================================================================

void kernel_init( void ) {
	// configure failover output
	driver_serial();

	// show kernel name, version, architecture and build time
	kernel_log( KERNEL_name" (v"KERNEL_version"."KERNEL_revision" "KERNEL_architecture" "KERNEL_language", build on "__DATE__" "__TIME__")\n" );

	// graphics-mode available?
	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STATIC_VIDEO_DEPTH_bit ) {
		// show error
		kernel_log( "Where are my testicles, Summer?" );

		// hold the door
		while( TRUE );
	}

	// retrieve file to execute
	kernel_init_cmd();

	// create binary memory map
	kernel_init_memory();

	// parse ACPI tables
	kernel_init_acpi();

	// recreate kernel's paging structures
	kernel_init_page();

	// reload new kernel environment paging array
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address - KERNEL_PAGE_mirror), "r" ((uintptr_t) KERNEL_CONTEXT_STACK_pointer) );

	// create Global Descriptor Table
	kernel_init_gdt();

	// create Interrupt Descriptor Table
	kernel_init_idt();

	// create Task queue
	kernel_init_task();

	// create streams
	kernel_init_stream();

	// share environment variables/functions
	kernel_init_env();

	// configure RTC
	kernel_init_rtc();

	// configure HPET
	// kernel_init_hpet();

	// configure PS2 controller
	driver_ps2_init();

	// create interprocess communication system
	kernel_init_ipc();

	// register all available data carriers
	kernel_init_storage();

	// prepare library subsystem
	kernel_init_library();

	// initialize other CPUs
	kernel_init_smp();

	// free up reclaimable memory
	kernel_init_free();

	// apply kernel modules
	kernel_init_daemon();

	// execute init process
	kernel_init_exec();

	// reload BSP processor
	kernel_init_ap();
}
