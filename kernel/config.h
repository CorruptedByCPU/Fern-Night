/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_name				"Fern-Night"
#define	KERNEL_version				"0"
#define	KERNEL_revision				"2167"
#define	KERNEL_architecture			"x86_64"
#define	KERNEL_language				"C"

// whole 510th entry of PML4 table
#define	KERNEL_CONTEXT_STACK_byte		(STATIC_PAGE_SIZE_byte << STATIC_MULTIPLE_BY_2_shift)
#define	KERNEL_CONTEXT_STACK_page		(KERNEL_CONTEXT_STACK_byte >> STATIC_PAGE_SIZE_shift)
#define	KERNEL_CONTEXT_STACK_address		(0xFFFFFF8000000000 - KERNEL_CONTEXT_STACK_byte)
#define	KERNEL_CONTEXT_STACK_pointer		(0xFFFFFF8000000000 - STATIC_PAGE_SIZE_byte)

#ifndef	KERNEL_GDT
	#include		"./gdt.h"
#endif

#ifndef	KERNEL_TSS
	#include		"./tss.h"
#endif

#ifndef	KERNEL_IDT
	#include		"./idt.h"
#endif

#define	KERNEL_STORAGE_limit			1

#define	KERNEL_STORAGE_TYPE_memory		0b00000001

#define	KERNEL_STREAM_SIZE_page			MACRO_PAGE_ALIGN_UP( LIB_SYS_STREAM_SIZE_byte ) >> STATIC_PAGE_SIZE_shift

#define	KERNEL_TSS_BSP_offset			7

#define	KERNEL_VIRTUAL_SIZE_page		1
#define KERNEL_VIRTUAL_SIZE_byte		(KERNEL_VIRTUAL_SIZE_page << STATIC_PAGE_SIZE_shift)

// this structure should be divisible by power of 2
struct KERNEL_STORAGE_STRUCTURE {
	_Alignas( uint64_t ) uint8_t		device_type;
	uint64_t				device_blocks;
	uint64_t 				device_first_block;
	struct KERNEL_STORAGE_STRUCTURE_FILE	(*storage_file)( uint64_t storage_id, uint8_t *path, uint8_t length );
	void					(*storage_read)( uint64_t storage_id, uint64_t file_id, uintptr_t target );
	uint8_t					reserved[ 24 ];
};

// this structure should be divisible by 8
struct KERNEL_STORAGE_STRUCTURE_FILE {
	uint64_t	id;
	uint64_t	size_byte;
};

struct	KERNEL_TSS_STRUCTURE {
	uint32_t	reserved;
	uint64_t	rsp0;
	uint8_t		unused[ 92 ];
} __attribute__( (packed) );

struct	KERNEL_TSS_STRUCTURE_ENTRY {
	uint16_t	limit_low;
	uint16_t	base_low;
	uint8_t		base_middle;
	uint8_t		access;
	uint8_t		flags_and_limit_high;
	uint8_t		base_high;
	uint32_t	base_64bit;
	uint32_t	reserved;
} __attribute__( (packed) );

struct KERNEL {
	// variables of Kernel management functions
	volatile uint64_t					cpu_count;

	// variables of Input devices
	uint8_t							device_mouse_status;
	uint16_t						device_mouse_x;
	uint16_t						device_mouse_y;
	uint16_t						device_keyboard[ 8 ];	// cache size of 8 keys

	// variables of Framebuffer functions
	uint32_t						*framebuffer_base_address;
	uint16_t						framebuffer_width_pixel;
	uint16_t						framebuffer_height_pixel;
	uint32_t						framebuffer_pitch_byte;
	int64_t							framebuffer_pid;

	// variables of GDT management functions
	struct KERNEL_STRUCTURE_GDT_HEADER			gdt_header;

	// variables of IDT management functions
	struct KERNEL_IDT_STRUCTURE_HEADER			idt_header;
	// functions of IDT management
	void							(*idt_mount)( uint8_t id, uint16_t type, uintptr_t address );

	// variables of I/O APIC management functions
	volatile struct KERNEL_STRUCTURE_IO_APIC_REGISTER	*io_apic_base_address;
	uint32_t						io_apic_irq_lines;
	uint8_t							io_apic_semaphore;

// variables of IPC management functions
struct LIB_SYS_STRUCTURE_IPC	*ipc_base_address;
uint8_t		ipc_semaphore;

	// variables of APIC management functions
	struct KERNEL_LAPIC_STRUCTURE				*lapic_base_address;
	uint32_t						lapic_last_id;

// variables of Library management functions
struct KERNEL_LIBRARY_STRUCTURE	*library_base_address;
uint32_t	*library_memory_map_address;

// Log management function
void		(*log)( const char *string, ... );

// variables of Memory management functions
uint32_t	*memory_base_address;
uint8_t		memory_page_semaphore;
// pointers of Memory management functions
void		(*memory_release)( uintptr_t address, uint64_t pages );

// variables of page management functions
uint64_t	*page_base_address;
uint64_t	page_total;
uint64_t	page_available;
uint64_t	page_limit;
uint64_t	page_array;
// pointers of Page management
void		(*page_deconstruction)( uintptr_t *pml4 );

// variables of Storage management functions
struct KERNEL_STORAGE_STRUCTURE	*storage_base_address;
uint8_t		storage_root_id;
uint8_t		storage_semaphore;

// variable of Stream management functions
struct KERNEL_STRUCTURE_STREAM	*stream_base_address;
uint8_t		stream_semaphore;
// pointers of Stream management functions
void		(*stream_release)( struct KERNEL_STRUCTURE_STREAM *stream );

	// variables of Task management functions
	struct KERNEL_STRUCTURE_TASK			*task_base_address;
	uintptr_t					*task_cpu_address;
	uint8_t						task_cpu_semaphore;
	uint8_t						task_semaphore;
	uint64_t					task_limit;
	uint64_t					task_count;
	int64_t						task_id;

// variables of Time management functions
volatile uint64_t					time_rdtsc;
volatile uint64_t					time_rtc;

	// variables of TSS management functions
	struct KERNEL_STRUCTURE_TSS	tss_table;
} __attribute__( (packed) );
