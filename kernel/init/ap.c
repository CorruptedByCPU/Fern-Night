/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

// external routine (assembly language)
extern void kernel_syscall();

void kernel_init_ap() {
	// reload kernel paging tables
	__asm__ volatile( "movq %0, %%cr3\nmovq %1, %%rsp" :: "r" ((uintptr_t) kernel -> page_base_address - KERNEL_PAGE_mirror), "r" ((uintptr_t) KERNEL_INIT_AP_STACK_pointer) );

	// reload the Global Descriptor Table
	__asm__ volatile( "lgdt (%0)" :: "r" (&kernel_gdt_header) );

	// reset to valid descriptor values
	kernel_gdt_descriptors();

	// reload the Interrupt Descriptor Table
	__asm__ volatile( "lidt (%0)" :: "r" (&kernel_idt_header) );

	// create a TSS descriptor for current BSP/logical processor
	uint8_t id = kernel_lapic_id();

	// Task State Segment descriptor properties
	struct KERNEL_TSS_STRUCTURE_ENTRY *tss = (struct KERNEL_TSS_STRUCTURE_ENTRY *) &kernel_gdt_base_address[ KERNEL_TSS_BSP_offset ];

	// insert the descriptor data for the BSP/logical processor
	tss[ id ].limit_low = sizeof( struct KERNEL_TSS_STRUCTURE );	// size of the Task State Segment array in Bytes
	tss[ id ].base_low = (uint64_t) &kernel_tss_table;		// TSS table address (bits 15..0)
	tss[ id ].base_middle = (uint64_t) &kernel_tss_table >> 16;	// TSS table address (bits 23..16)
	tss[ id ].access = 0b10001001;					// TSS descriptor access attributes
	tss[ id ].base_high = (uint64_t) &kernel_tss_table >> 24;	// TSS table address (bits 31..24)
	tss[ id ].base_64bit = (uint64_t) &kernel_tss_table >> 32;	// TSS table address (bits 63..32)

	// set the TSS descriptor for the BSP/logical processor
	__asm__ volatile( "ltr %%ax" :: "a" ((uintptr_t) &tss[ id ] & ~STATIC_PAGE_mask) );

	// disable Emulation, enable Monitoring
	__asm__ volatile( "movq %cr0, %rax\nand $0xFFFB, %ax\nor $0x02, %ax\nmovq %rax, %cr0" );

	// reset FPU state
	__asm__ volatile( "fninit" );

	// enable OSXSAVE, OSFXSR support
	__asm__ volatile( "movq %cr4, %rax\norq $0b1000000011000000000, %rax\nmovq %rax, %cr4" );

	// enable X87, SSE, AVX support
	__asm__ volatile( "xorq %rcx, %rcx\nxgetbv\nor $0x3, %eax\nxsetbv" );

	//--------------------------------------------------------------------------
	// enable syscall/sysret support
	__asm__ volatile( "movl $0xC0000080, %ecx\nrdmsr\nor $1, %eax\nwrmsr" );	// enable SCE flag
	// set code/stack segments for kernel and process
	__asm__ volatile( "wrmsr" :: "a" (EMPTY), "d" (0x00180008), "c" (KERNEL_INIT_AP_MSR_STAR) );
	// set the kernel entry routin
	__asm__ volatile( "wrmsr" :: "a" ((uintptr_t) kernel_syscall), "d" ((uintptr_t) kernel_syscall >> STATIC_MOVE_HIGH_TO_EAX_shift), "c" (KERNEL_INIT_AP_MSR_LSTAR) );
	// disable IF flag and DF after calling syscall
	__asm__ volatile( "wrmsr" :: "a" ((uint32_t) KERNEL_TASK_EFLAGS_if | KERNEL_TASK_EFLAGS_df), "d" (EMPTY ), "c" (KERNEL_INIT_AP_MSR_EFLAGS) );
	//--------------------------------------------------------------------------

	// add the currently processed process to the processor task list
	kernel -> task_ap_address[ kernel_lapic_id() ] = (uintptr_t) kernel -> task_queue_address;

	// initialize the LAPIC of the logical processor
	kernel_lapic_init();

	// reload cpu cycle counter in LAPIC controller
	kernel_lapic_reload();

	// accept the current interrupt call
	kernel_lapic_accept();

	// AP initialized
	kernel -> cpu_count++;

	// enable interrupt handling
	__asm__ volatile( "sti" );

	// wait for miracle :]
	while( TRUE );
}
