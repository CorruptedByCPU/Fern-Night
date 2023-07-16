;===============================================================================
;Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;===============================================================================

; get pointer from APIC exception handling function
extern	kernel_task

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; share routine
global	kernel_task_entry

; align routine to full address
align	0x08,	db	0x00
kernel_task_entry:
	; turn off Interrupts Flag
	cli

	; turn off Direction Flag
	cld

	; keep original registers
	push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rsi
	push	rdi
	push	rbp
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15

	; keep "floating point" registers
	mov	rbp,	0xFFFFFF8000000000 - 0x1000
	FXSAVE64	[rbp]

	; execute exception handler
	call	kernel_task

	; restore "floating point" registers
	mov	rbp,	0xFFFFFF8000000000 - 0x1000
	FXRSTOR64	[rbp]

	; restore ogirinal registers
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rbp
	pop	rdi
	pop	rsi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax

	; return from the procedure
	iretq