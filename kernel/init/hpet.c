/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_hpet() {
	// connect HPET controller Timer 2 to interrupt handler
	kernel_idt_mount( KERNEL_IDT_IRQ_offset + KERNEL_HPET_TIMER_UPTIME_irq, KERNEL_IDT_TYPE_irq, (uintptr_t) kernel_hpet_uptime_entry );

	// connect interrupt vector from IDT table in I/O APIC controller
	kernel_io_apic_connect( KERNEL_IDT_IRQ_offset + KERNEL_HPET_TIMER_UPTIME_irq, KERNEL_HPET_IO_APIC_register );

	// configure Timer 2 as uptime counter
	struct KERNEL_HPET_STRUCTURE_TIMER *timer_uptime = (struct KERNEL_HPET_STRUCTURE_TIMER *) ((uintptr_t) kernel -> hpet_base_address + KERNEL_HPET_TIMER_offset + (sizeof( struct KERNEL_HPET_STRUCTURE_TIMER ) * 2) );

		// by default not configured
		uint64_t bits = EMPTY;

		// IRQ number of IDT and I/O APIC
		bits |= (uint64_t) KERNEL_HPET_TIMER_UPTIME_irq << 9;

		// allow to set own periodic value
		bits |= (uint64_t) 1 << 6;

		// periodic type interrupts
		bits |= (uint64_t) 1 << 3;

		// enable interrupts
		bits |= (uint64_t) 1 << 2;

		// edge triggered interrupts
		bits |= (uint64_t) 0 << 1;

		// write configuration
		timer_uptime -> configuration_and_capabilities = bits;

		// interval every 1ms
		timer_uptime -> comparator = (uint64_t) 100000;


	// enable HPET controller
	volatile uint64_t configuration = kernel -> hpet_base_address -> general_configuration;
	configuration |= 1;
	kernel -> hpet_base_address -> general_configuration = configuration;
}