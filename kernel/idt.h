/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IDT
	#define	KERNEL_IDT

	#define	KERNEL_IDT_IRQ_offset			0x20
	#define	KERNEL_IDT_TYPE_exception		0x8E00
	#define	KERNEL_IDT_TYPE_irq			0x8F00
	#define	KERNEL_IDT_TYPE_isr			0xEF00

	struct	KERNEL_IDT_STRUCTURE_ENTRY {
		uint16_t	base_low;
		uint16_t	gdt_descriptor;
		uint16_t	type;
		uint16_t	base_middle;
		uint32_t	base_high;
		uint32_t	reserved;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_EXCEPTION {
		uint64_t	cr2;
		uint64_t	r15;
		uint64_t	r14;
		uint64_t	r13;
		uint64_t	r12;
		uint64_t	r11;
		uint64_t	r10;
		uint64_t	r9;
		uint64_t	r8;
		uint64_t	rbp;
		uint64_t	rdi;
		uint64_t	rsi;
		uint64_t	rdx;
		uint64_t	rcx;
		uint64_t	rbx;
		uint64_t	rax;
		uint64_t	id;
		uint64_t	error_code;
		uintptr_t	rip;
		uint64_t	cs;
		uint64_t	eflags;
		uintptr_t	rsp;
		uint64_t	ss;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_HEADER {
		uint16_t	size;
		uint64_t	address;
	} __attribute__( (packed) );

	struct	KERNEL_IDT_STRUCTURE_RETURN {
		uintptr_t	rip;
		uint64_t	cs;
		uint64_t	eflags;
		uintptr_t	rsp;
		uint64_t	ss;
	} __attribute__( (packed) );

	void kernel_idt_mount( uint8_t id, uint16_t type, uint64_t procedure_address );
	void kernel_idt_exception( struct KERNEL_IDT_STRUCTURE_EXCEPTION *exception );
	void kernel_idt_interrupt_default( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt );
	void kernel_idt_spurious_interrupt( struct KERNEL_IDT_STRUCTURE_RETURN *interrupt );
#endif
