;=================================================================================
; Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
;=================================================================================

;------------------------------------------------------------------------------
; get pointers from kernel environment
;------------------------------------------------------------------------------
extern	kernel_service_exit
extern	kernel_service_framebuffer
extern	kernel_task_pid
extern	kernel_service_memory_alloc
extern	kernel_service_memory_release
extern	kernel_service_driver_mouse
extern	kernel_service_storage_read
extern	kernel_exec
extern	kernel_service_ipc_send
extern	kernel_service_ipc_receive
extern	kernel_service_memory_share
extern	driver_ps2_keyboard_key_read
extern	kernel_service_task_status
extern	kernel_stream_out
extern	kernel_stream_in
extern	driver_serial_char
extern	driver_serial_string
extern	driver_serial_value
extern	driver_rtc_time
extern	kernel_stream_set
extern	kernel_stream_get
extern	kernel_service_sleep
extern	kernel_service_uptime
extern	kernel_stream_out_value
extern	kernel_service_task
extern	kernel_service_memory
extern	kernel_service_thread

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; share routines and list
;------------------------------------------------------------------------------
global	kernel_syscall
global	kernel_irq
;------------------------------------------------------------------------------
global	kernel_service_list
;------------------------------------------------------------------------------

; information for linker
section	.rodata

; align routine to full address
align	0x08,	db	0x00
kernel_service_list:
	dq	kernel_service_exit
	dq	kernel_service_framebuffer
	dq	kernel_service_memory_alloc
	dq	kernel_service_memory_release
	dq	kernel_task_pid
	dq	kernel_service_driver_mouse
	dq	kernel_service_storage_read
	dq	kernel_exec
	dq	kernel_service_ipc_send
	dq	kernel_service_ipc_receive
	dq	kernel_service_memory_share
	dq	driver_ps2_keyboard_key_read
	dq	kernel_service_task_status
	dq	kernel_stream_out
	dq	kernel_stream_in
	dq	driver_serial_char
	dq	driver_serial_string
	dq	driver_serial_value
	dq	driver_rtc_time
	dq	kernel_stream_set
	dq	kernel_stream_get
	dq	kernel_service_sleep
	dq	kernel_service_uptime
	dq	kernel_stream_out_value
	dq	kernel_service_task
	dq	kernel_service_memory
	dq	kernel_service_thread
kernel_service_list_end:

; 64 bit procedure code
[BITS 64]

; information for linker
section	.text

; align routine to full address
align	0x08,	db	0x00
kernel_syscall:
	; keep RIP and EFLAGS registers of process
	xchg	qword [rsp + 0x08],	rcx
	xchg	qword [rsp],	r11

	; feature available?
	cmp	rax,	(kernel_service_list_end - kernel_service_list) / 0x08
	jbe	.available	; yes

	; no kernel feature selected
	mov	rax,	-1

	; go back to process
	jmp	.return

.available:
	; preserve original registers
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

.wait:
	; if function is not available yet
	cmp	qword [kernel_service_list + rax * 0x08], 0x0000000000000000
	je	.wait	; wait for it

	; execute kernel function according to parameter in RAX
	call	qword [kernel_service_list + rax * 0x08]

	; restore original registers
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

.return:
	; restore the RIP and EFLAGS registers of the process
	xchg	qword [rsp],	r11
	xchg	qword [rsp + 0x08],	rcx

	; return to process code
	o64	sysret

; align routine to full address
align	0x08,	db	0x00
kernel_irq:
	; feature available?
	cmp	rax,	(kernel_service_list_end - kernel_service_list) / 0x08
	jbe	.available	; yes

	; no kernel feature selected
	mov	rax,	-1

	; go back to process
	jmp	.return

.available:
	; preserve original registers
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

	; execute kernel function according to parameter in RAX
	call	qword [kernel_service_list + rax * 0x08]

	; restore original registers
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

.return:
	; return to process code
	iretq
