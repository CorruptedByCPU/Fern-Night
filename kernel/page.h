/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	KERNEL_PAGE
	#define	KERNEL_PAGE

	const uint64_t	KERNEL_PAGE_SIZE_entry	= 512;

	const uint64_t	KERNEL_PAGE_PML1_SIZE_byte	= KERNEL_PAGE_SIZE_entry * STATIC_PAGE_SIZE_byte;
	const uint64_t	KERNEL_PAGE_PML2_SIZE_byte	= KERNEL_PAGE_SIZE_entry * KERNEL_PAGE_PML1_SIZE_byte;
	const uint64_t	KERNEL_PAGE_PML3_SIZE_byte	= KERNEL_PAGE_SIZE_entry * KERNEL_PAGE_PML2_SIZE_byte;
	const uint64_t	KERNEL_PAGE_PML4_SIZE_byte	= KERNEL_PAGE_SIZE_entry * KERNEL_PAGE_PML3_SIZE_byte;

	#define	KERNEL_PAGE_mirror			0xFFFF800000000000

	#define	KERNEL_PAGE_FLAG_present		1 << 0
	#define	KERNEL_PAGE_FLAG_write			1 << 1
	#define	KERNEL_PAGE_FLAG_user			1 << 2
	#define	KERNEL_PAGE_FLAG_write_through		1 << 3
	#define	KERNEL_PAGE_FLAG_cache_disable		1 << 4
	#define	KERNEL_PAGE_FLAG_length			1 << 7
	#define	KERNEL_PAGE_FLAG_process		1 << 9
	#define	KERNEL_PAGE_FLAG_shared			1 << 10
	#define	KERNEL_PAGE_FLAG_library		1 << 11

	#define	KERNEL_PAGE_PML5_mask			0xFFFF000000000000

	#define	KERNEL_PAGE_ENTRY_stack			223
	#define	KERNEL_PAGE_ENTRY_stack_context		510

	uintptr_t kernel_page_address( uintptr_t *pml4, uintptr_t address );
	uint8_t kernel_page_alloc( uintptr_t *pml4, uintptr_t address, uint64_t pages, uint16_t flags );
	uintptr_t kernel_page_clean( uintptr_t address, uint64_t count );
	void kernel_page_deconstruction( uintptr_t *pml4 );
	uint8_t kernel_page_flags( uint64_t *pml4, uint64_t address, uint64_t pages, uint16_t flags );
	uint8_t kernel_page_map( uint64_t *pml4, uintptr_t source, uintptr_t target, uint64_t count, uint16_t flags );
	void kernel_page_merge( uintptr_t *pml4_source, uintptr_t *pml4_target );
	uintptr_t kernel_page_remove( uintptr_t *pml4, uintptr_t address );
#endif
