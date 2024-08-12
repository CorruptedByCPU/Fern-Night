/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_LAPIC
	#define	KERNEL_LAPIC

	#define	KERNEL_LAPIC_Hz					1024

	#define	KERNEL_LAPIC_IRQ_number				0x20
	#define	KERNEL_LAPIC_INIT_READY_bit			12
	#define	KERNEL_LAPIC_DF_FLAG_flat_mode			0xFFFFFFFF
	#define	KERNEL_LAPIC_LD_FLAG_target_cpu			0x01000000
	#define	KERNEL_LAPIC_SIV_FLAG_enable_apic		0b00000000000000000000000100000000
	#define	KERNEL_LAPIC_SIV_FLAG_spurious_vector3		0b00000000000000000000000011111111
	#define	KERNEL_LAPIC_LVT_TR_FLAG_mask_interrupts	0b00000000000000010000000000000000
	#define	KERNEL_LAPIC_TDC_divide_by_1			0b111

	struct	KERNEL_LAPIC_STRUCTURE {
		uint8_t		reserved_0[ 32 ];
		uint32_t	volatile id;	// 0x20
		uint8_t		reserved_1[ 92 ];
		uint32_t	tp;	// 0x80	Task Priority
		uint8_t		reserved_2[ 44 ];
		uint32_t	eoi;	// 0xB0	End Of Interrupt
		uint8_t		reserved_3[ 28 ];
		uint32_t	ld;	// 0xD0	Logical Destination
		uint8_t		reserved_4[ 12 ];
		uint32_t	df;	// 0xE0	Destination Format
		uint8_t		reserved_5[ 12 ];
		uint32_t	siv;	// 0xF0	Spurious Interrupt Vector
		uint8_t		reserved_6[ 524 ];
		uint32_t	icl;	// 0x0300	Interrupt Command Low
		uint8_t		reserved_7[ 12 ];
		uint32_t	ich;	// 0x0310	Interrupt Command High
		uint8_t		reserved_8[ 12 ];
		uint32_t	lvt;	// 0x0320	Local Vector Timer
		uint8_t		reserved_9[ 92 ];
		uint32_t	tic;	// 0x0380	Timer - Initial Count
		uint8_t		reserved_A[ 12 ];
		uint32_t	tcc;	// 0x0390	Timer - Current Count
		uint8_t		reserved_B[ 76 ];
		uint32_t	tdc;	// 0x03E0	Timer - Divide Configuration
	} __attribute__( (packed) );

	void kernel_lapic_reload();
	__attribute__ ((preserve_most)) void kernel_lapic_accept();
	uint8_t kernel_lapic_id();
	void kernel_lapic_init();
#endif
