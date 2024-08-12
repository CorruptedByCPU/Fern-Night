/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#define	KERNEL_name				"Fern-Night"
#define	KERNEL_version				"0"
#define	KERNEL_revision				"2166"
#define	KERNEL_architecture			"x86_64"
#define	KERNEL_language				"C"

// whole 510th entry of PML4 table
#define	KERNEL_CONTEXT_STACK_byte		(STATIC_PAGE_SIZE_byte << STATIC_MULTIPLE_BY_2_shift)
#define	KERNEL_CONTEXT_STACK_page		(KERNEL_CONTEXT_STACK_byte >> STATIC_PAGE_SIZE_shift)
#define	KERNEL_CONTEXT_STACK_address		(0xFFFFFF8000000000 - KERNEL_CONTEXT_STACK_byte)
#define	KERNEL_CONTEXT_STACK_pointer		(0xFFFFFF8000000000 - STATIC_PAGE_SIZE_byte)

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

struct KERNEL_STRUCTURE {
	// variables of SMP Management functions
	volatile uint64_t	cpu_count;

	// variables of PS2 controller management functions
	uint16_t	driver_ps2_mouse_x;
	uint16_t	driver_ps2_mouse_y;
	int64_t		driver_ps2_mouse_x_absolute;
	int64_t		driver_ps2_mouse_y_absolute;
	uint8_t		driver_ps2_mouse_status;

	// variables of video memory
	uint32_t	*framebuffer_base_address;
	uint16_t	framebuffer_width_pixel;
	uint16_t	framebuffer_height_pixel;
	uint32_t	framebuffer_scanline_byte;
	uint64_t	framebuffer_manager;

	// variables of HPET management functions
	volatile struct KERNEL_HPET_STRUCTURE_REGISTER	*hpet_base_address;
	uint64_t	hpet_microtime;

	// variables of I/O APIC management functions
	volatile struct KERNEL_IO_APIC_STRUCTURE_REGISTER	*io_apic_base_address;

	// variables of IPC management functions
	struct LIB_SYS_STRUCTURE_IPC	*ipc_base_address;
	uint8_t		ipc_semaphore;

	// variables of APIC management functions
	struct KERNEL_LAPIC_STRUCTURE	*lapic_base_address;
	uint32_t		lapic_last_id;

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
	struct KERNEL_STREAM_STRUCTURE	*stream_base_address;
	uint8_t		stream_semaphore;
	// pointers of Stream management functions
	void		(*stream_release)( struct KERNEL_STREAM_STRUCTURE *stream );

	// variables of Task management functions
	struct KERNEL_TASK_STRUCTURE	*task_queue_address;
	uintptr_t	*task_ap_address;
	uint8_t		task_queue_semaphore;
	uint64_t	task_id;
	uint64_t	task_count;
	uint8_t		task_cpu_semaphore;
} __attribute__( (packed) );
