/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_idt_mount( uint8_t id, uint16_t type, uint64_t procedure_address ) {
	// interrupt type
	kernel_idt_base_address[ id ].type = type;

	// address of code descriptor that runs procedure
	kernel_idt_base_address[ id ].gdt_descriptor = sizeof( ( ( struct KERNEL_INIT_GDT_STRUCTURE *) 0 ) -> cs_ring0 );

	// address of exception handler
	kernel_idt_base_address[ id ].base_low = (uint64_t) procedure_address;
	kernel_idt_base_address[ id ].base_middle = (uint64_t) procedure_address >> STATIC_MOVE_HIGH_TO_AX_shift;
	kernel_idt_base_address[ id ].base_high = (uint64_t) procedure_address >> STATIC_MOVE_HIGH_TO_EAX_shift;
}

void kernel_idt_exception( struct KERNEL_IDT_STRUCTURE_EXCEPTION *exception ) {
	// exception approved?
	uint8_t approved = FALSE;	// by default no

	// properties of current task
	struct KERNEL_TASK_STRUCTURE *task = kernel_task_active();

	// show type of exception
	kernel_log( "Exception: " );
	switch( exception -> id ) {
		case 0: { kernel_log( "Divide-by-zero Error" ); break; }
		case 1: { kernel_log( "Debug" ); break; }
		case 3: { kernel_log( "Breakpoint" ); break; }
		case 4: { kernel_log( "Overflow" ); break; }
		case 5: { kernel_log( "Bound Range Exceeded" ); break; }
		case 6: { kernel_log( "Invalid Opcode" ); break; }
		case 7: { kernel_log( "Device Not Available" ); break; }
		case 8: { kernel_log( "Double Fault" ); break; }
		case 9: { kernel_log( "Coprocessor Segment Overrun" ); break; }
		case 10: { kernel_log( "Invalid TSS" ); break; }
		case 11: { kernel_log( "Segment Not Present" ); break; }
		case 12: { kernel_log( "Stack-Segment Fault" ); break; }
		case 13: { kernel_log( "General Protection Fault" ); break; }
		case 14: {
			// show type of exception
			kernel_log( "Page Fault" );
			
			// exception for process stack space?
			if( MACRO_PAGE_ALIGN_UP( exception -> cr2 ) == KERNEL_EXEC_STACK_pointer - (task -> stack << STATIC_PAGE_SIZE_shift) ) {
				// describe additional space under process stack
				kernel_page_alloc( (uintptr_t *) task -> cr3, KERNEL_EXEC_STACK_pointer - (++task -> stack << STATIC_PAGE_SIZE_shift), 1, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write | KERNEL_PAGE_FLAG_user | KERNEL_PAGE_FLAG_process );

				// show solution explanation
				kernel_log( " [accepted: stack extension]\n" );

				// go back to process
				approved = TRUE;
			}

			// exception parsed
			break;
		}
		case 16: { kernel_log( "x87 Floating-Point" ); break; }
		case 17: { kernel_log( "Alignment Check" ); break; }
		case 18: { kernel_log( "Machine Check" ); break; }
		case 19: { kernel_log( "SIMD Floating-Point" ); break; }
		case 20: { kernel_log( "Virtualization" ); break; }
		case 21: { kernel_log( "Control Protection" ); break; }
		case 28: { kernel_log( "Hypervisor Injection" ); break; }
		case 29: { kernel_log( "VMM Communication" ); break; }
		case 30: { kernel_log( "Security" ); break; }
		default: { kernel_log( "{unknown}" ); break; }
	}

	// no solution for exception?
	if( ! approved ) {
		// show name of process
		kernel_log( " [%d:%s at 0x%x]\n", task -> pid, task -> name, exception -> rip );

		// DEBUG ---------------------------------------------------------------
		MACRO_DEBUF();
		__asm__ volatile( "popq %rbx" );
		while( TRUE ) {}
		//======================================================================
	}
}

void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt ) {
	// tell APIC of current logical processor that the hardware interrupt is being handled properly
	kernel -> lapic_base_address -> eoi = EMPTY;
}
