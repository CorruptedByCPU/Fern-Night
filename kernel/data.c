/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// kernel environment variables/functions
struct KERNEL *kernel = EMPTY;

// variables of GDT management functions
struct KERNEL_INIT_GDT_STRUCTURE_ENTRY	*kernel_gdt_base_address;
struct KERNEL_INIT_GDT_STRUCTURE_HEADER	kernel_gdt_header;

// varuables of IDT management functions
struct KERNEL_IDT_STRUCTURE_ENTRY	*kernel_idt_base_address;
struct KERNEL_IDT_STRUCTURE_HEADER	kernel_idt_header;

// variables of TSS management functions
struct KERNEL_TSS_STRUCTURE		kernel_tss_table;