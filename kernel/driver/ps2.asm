;===============================================================================
;Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;===============================================================================

; get pointers from driver handling functions
extern	driver_ps2_keyboard
extern	driver_ps2_mouse

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routines
global	driver_ps2_mouse_entry
global	driver_ps2_keyboard_entry

; align routine to full address
align	0x08,	db	0x00
driver_ps2_mouse_entry:
	; preserve original register
	push	r11

	; execute driver handler
	call	driver_ps2_mouse

	; restore original register
	pop	r11

	; return from the procedure
	iretq

; align routine to full address
align	0x08,	db	0x00
driver_ps2_keyboard_entry:
	; preserve original register
	push	r11
	
	; execute driver handler
	call	driver_ps2_keyboard

	; restore original register
	pop	r11

	; return from the procedure
	iretq
