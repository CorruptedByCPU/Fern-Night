/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

// external routine (assembly language)
extern void kernel_task_entry( void );

void kernel_init_task() {
	// make space for task queue
	kernel -> task_queue_address = (struct KERNEL_TASK_STRUCTURE *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( KERNEL_TASK_limit * sizeof( struct KERNEL_TASK_STRUCTURE ) ) >> STATIC_PAGE_SIZE_shift );

	// prepare a space for list of tasks assigned to BSP/logical processors
	kernel -> task_ap_address = (uintptr_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( limine_smp_request.response -> cpu_count << STATIC_MULTIPLE_BY_QWORD_shift ) >> STATIC_PAGE_SIZE_shift );

	// properties of first job in queue
	kernel -> task_queue_address -> flags = KERNEL_TASK_FLAG_secured;	// record seized and processed (by BSP processor)
	kernel -> task_queue_address -> cr3 = (uintptr_t) kernel -> page_base_address;	// address of task page table
	kernel -> task_queue_address -> memory_map = kernel -> memory_base_address;	// address of task binary memory map

	// set BSP's pointer to job being performed by BSP to first in the queue
	kernel -> task_ap_address[ kernel_lapic_id() ] = (uintptr_t) kernel -> task_queue_address;

	// number of tasks in queue
	kernel -> task_count++;

	// attach task switch routine to APIC timer interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset, KERNEL_IDT_TYPE_irq, (uintptr_t) kernel_task_entry );
}
