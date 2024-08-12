/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_INIT_IDT
	#define	KERNEL_INIT_IDT

	// external routines (assembly language)
	extern void kernel_idt_exception_divide_by_zero( void );
	extern void kernel_idt_exception_debug( void );
	extern void kernel_idt_exception_breakpoint( void );
	extern void kernel_idt_exception_overflow( void );
	extern void kernel_idt_exception_boud_range_exceeded( void );
	extern void kernel_idt_exception_invalid_opcode( void );
	extern void kernel_idt_exception_device_not_available( void );
	extern void kernel_idt_exception_double_fault( void );
	extern void kernel_idt_exception_coprocessor_segment_overrun( void );
	extern void kernel_idt_exception_invalid_tss( void );
	extern void kernel_idt_exception_segment_not_present( void );
	extern void kernel_idt_exception_stack_segment_fault( void );
	extern void kernel_idt_exception_general_protection_fault( void );
	extern void kernel_idt_exception_page_fault( void );
	extern void kernel_idt_exception_x87_floating_point( void );
	extern void kernel_idt_exception_alignment_check( void );
	extern void kernel_idt_exception_machine_check( void );
	extern void kernel_idt_exception_simd_floating_point( void );
	extern void kernel_idt_exception_virtualization( void );
	extern void kernel_idt_exception_control_protection( void );
	extern void kernel_idt_exception_hypervisor_injection( void );
	extern void kernel_idt_exception_vmm_communication( void );
	extern void kernel_idt_exception_security( void );
	extern void kernel_idt_exception_triple_fault( void );
	extern void kernel_idt_spurious_interrupt( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt );

	// external routine (assembly language)
	extern void kernel_irq( void );

	void kernel_init_idt();
#endif