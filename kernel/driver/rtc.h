/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	DRIVER_RTC
	#define	DRIVER_RTC

	// external routine (assembly language)
	extern void driver_rtc_entry( void );

	#define	DRIVER_RTC_IRQ_number					0x08
	#define	DRIVER_RTC_IO_APIC_register				KERNEL_IO_APIC_iowin + (DRIVER_RTC_IRQ_number * 0x02)

	#define	DRIVER_RTC_Hz						1024

	#define	DRIVER_RTC_PORT_command					0x0070
	#define	DRIVER_RTC_PORT_data					0x0071

	#define	DRIVER_RTC_REGISTER_seconds				0x00
	#define	DRIVER_RTC_REGISTER_minutes				0x02
	#define	DRIVER_RTC_REGISTER_hour				0x04
	#define	DRIVER_RTC_REGISTER_weekday				0x06
	#define	DRIVER_RTC_REGISTER_day_of_month			0x07
	#define	DRIVER_RTC_REGISTER_month				0x08
	#define	DRIVER_RTC_REGISTER_year				0x09
	
	#define	DRIVER_RTC_STATUS_REGISTER_A				0x0A
	#define	DRIVER_RTC_STATUS_REGISTER_B				0x0B
	#define	DRIVER_RTC_STATUS_REGISTER_C				0x0C

	#define	DRIVER_RTC_STATUS_REGISTER_A_rate			0b00000110	// 1024 Hz
	#define	DRIVER_RTC_STATUS_REGISTER_A_divider			0b00100000	// 32768 kHz
	#define	DRIVER_RTC_STATUS_REGISTER_A_update_in_progress		0b10000000

	#define	DRIVER_RTC_STATUS_REGISTER_B_daylight_savings		0b00000001
	#define	DRIVER_RTC_STATUS_REGISTER_B_24_hour_mode		0b00000010
	#define	DRIVER_RTC_STATUS_REGISTER_B_data_mode_binary		0b00000100	// no BCD
	#define	DRIVER_RTC_STATUS_REGISTER_B_square_wave_output		0b00001000
	#define	DRIVER_RTC_STATUS_REGISTER_B_update_ended_interrupt	0b00010000	// announce with interrupt when controller exited modification mode
	#define	DRIVER_RTC_STATUS_REGISTER_B_alarm_interrupt		0b00100000
	#define	DRIVER_RTC_STATUS_REGISTER_B_periodic_interrupt		0b01000000
	#define	DRIVER_RTC_STATUS_REGISTER_B_update_in_progress		0b10000000

	void driver_rtc( void );
	void driver_rtc_init( void );
	uint64_t driver_rtc_time( void );
	uint8_t driver_rtc_register( uint8_t mode );
#endif
