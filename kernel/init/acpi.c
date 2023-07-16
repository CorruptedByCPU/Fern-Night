/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

void kernel_init_acpi() {
	// RSDP pointer available?
	if( limine_rsdp_request.response == NULL || ! limine_rsdp_request.response -> address ) {
		// show error
		kernel_log( "Hello Darkness, My Old Friend." );

		// hold the door
		while( TRUE );
	}

	// RSDP or XSDP header properties
	struct KERNEL_INIT_ACPI_STRUCTURE_RSDP_OR_XSDP_HEADER *rsdp_or_xsdp_header = (struct KERNEL_INIT_ACPI_STRUCTURE_RSDP_OR_XSDP_HEADER *) limine_rsdp_request.response -> address;

	// amount of entries
	uint64_t list_length = EMPTY;

	// pointer to list depending on revision
	uint32_t *list_rsdt_address = EMPTY;
	uint64_t *list_xsdt_address = EMPTY;

	// check revision number of RSDP header
	if( rsdp_or_xsdp_header -> revision == EMPTY ) {
		// show message regarding ACPI
		kernel_log( "RSDT (Root System Description Pointer) found.\n" );

		// RSDT header properties
		struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *rsdt = (struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *) ((uintptr_t) rsdp_or_xsdp_header -> rsdt_address);
	
		// amount of entries
		list_length = (rsdt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT )) >> STATIC_DIVIDE_BY_4_shift;

		// pointer to list of RSDT entries
		list_rsdt_address = (uint32_t *) ((uintptr_t) rsdp_or_xsdp_header -> rsdt_address + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT ));
	} else {
		// show message regarding ACPI
		kernel_log( "XSDT (eXtended System Descriptor Table) found.\n" );

		// XSDT header properties
		struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *xsdt = (struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT *) ((uintptr_t) rsdp_or_xsdp_header -> xsdt_address);

		// amount of entries
		list_length = (xsdt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT )) >> STATIC_DIVIDE_BY_8_shift;

		// pointer to list of XSDT entries
		list_xsdt_address = (uint64_t *) ((uintptr_t) rsdp_or_xsdp_header -> xsdt_address + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_DEFAULT ));
	}

	// do recon on all entries of list
	for( uint64_t i = 0; i < list_length; i++ ) {
		// initialize local variable
		uint64_t entry = EMPTY;

		// get address of table from a given entry on list
		if( rsdp_or_xsdp_header -> revision == EMPTY )
			// with RSDT in case of ACPI 1.0
			entry = list_rsdt_address[ i ];
		else
			// or XSDT in case of ACPI 2.0+
			entry = list_xsdt_address[ i ];

		// if entry contains an MADT signature (Multiple APIC Description Table)
		struct KERNEL_INIT_ACPI_STRUCTURE_MADT *madt = (struct KERNEL_INIT_ACPI_STRUCTURE_MADT *) entry;
		if( madt -> signature == KERNEL_INIT_ACPI_MADT_signature ) {
			// store base address and size of LAPIC entry
			kernel -> lapic_base_address = (struct KERNEL_LAPIC_STRUCTURE *) (madt -> lapic_address | KERNEL_PAGE_mirror);

			// show message regarding LAPIC
			kernel_log( " LAPIC base address 0x%x\n", (uint64_t) kernel -> lapic_base_address );

			// length of MADT list
			uint64_t size = (uint32_t) madt -> length - sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_MADT );
		
			// pointer of MADT list
			uint8_t *list = (uint8_t *) entry + sizeof( struct KERNEL_INIT_ACPI_STRUCTURE_MADT );

			// process all MADT list entries
			while( size ) {
				// get size of entry being processed
				struct KERNEL_INIT_ACPI_STRUCTURE_MADT_ENTRY *entry = (struct KERNEL_INIT_ACPI_STRUCTURE_MADT_ENTRY *) list;
				uint8_t entry_length = (uint8_t) entry -> length;

				// I/O APIC entry found?
				struct KERNEL_INIT_ACPI_STRUCTURE_IO_APIC *io_apic = (struct KERNEL_INIT_ACPI_STRUCTURE_IO_APIC *) list;
				if( io_apic -> type == KERNEL_INIT_ACPI_APIC_TYPE_io_apic )
					// I/O APIC supports interrupt vectors 0+?
					if( io_apic -> gsib == EMPTY ) {
						// store base address of I/O APIC
						kernel -> io_apic_base_address = (struct KERNEL_IO_APIC_STRUCTURE_REGISTER *) (io_apic -> base_address | KERNEL_PAGE_mirror);

						// show message regarding I/O APIC
						kernel_log( " I/O APIC base address 0x%x\n", (uint64_t) kernel -> io_apic_base_address );
					}

				// check next entry on list
				list += entry_length;
				size -= entry_length;
			}
		}

		// if entry contains an HPET signature (High-Precision Event Timer)
		struct KERNEL_INIT_ACPI_STRUCTURE_HPET *hpet = (struct KERNEL_INIT_ACPI_STRUCTURE_HPET *) entry;
		if( hpet -> signature == KERNEL_INIT_ACPI_HPET_signature ) {
			// store base address of I/O APIC
			kernel -> hpet_base_address = (struct KERNEL_HPET_STRUCTURE_REGISTER *) (hpet -> base_address | KERNEL_PAGE_mirror);

			// show message regarding I/O APIC
			kernel_log( " HPET base address 0x%x\n", (uint64_t) kernel -> hpet_base_address );
		}

	}
}
