/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

inline void lib_sys_return_empty() {
	// call the kernel function
	__asm__ volatile( "push %%rax\npush %%rcx\npush %%r11\nsyscall\npop %%r11\npop %%rcx\npop %%rax" :: );
}

inline uint8_t lib_sys_return_bool() {
	// initialize local variable
	uint8_t rax;

	// call the kernel function
	__asm__ volatile(
		"push	%%rcx	\n"
		"push	%%r11	\n"
		"syscall	\n"
		"pop	%%r11	\n"
		"pop	%%rcx	\n"
		: "=a" (rax)
	);

	// return TRUE/FALSE
	return rax;
}

inline int64_t lib_sys_return_value() {
	// initialize local variable
	int64_t rax;

	// call the kernel function
	__asm__ volatile(
		"push	%%rcx	\n"
		"push	%%r11	\n"
		"syscall	\n"
		"pop	%%r11	\n"
		"pop	%%rcx	\n"
		: "=a" (rax)
	);

	// return value
	return rax;
}

inline uint64_t lib_sys_return_value_unsigned() {
	// initialize local variable
	uint64_t rax;

	// call the kernel function
	__asm__ volatile(
		"push	%%rcx	\n"
		"push	%%r11	\n"
		"syscall	\n"
		"pop	%%r11	\n"
		"pop	%%rcx	\n"
		: "=a" (rax)
	);

	// return unsigned value
	return rax;
}

inline uintptr_t lib_sys_return_pointer() {
	// initialize local variable
	uintptr_t rax;

	// call the kernel function
	__asm__ volatile(
		"push	%%rcx	\n"
		"push	%%r11	\n"
		"syscall	\n"
		"pop	%%r11	\n"
		"pop	%%rcx	\n"
		: "=a" (rax)
	);

	// return a pointer
	return rax;
}

//------------------------------------------------------------------------------

inline void lib_sys_int_return_empty() {
	// call the kernel function
	__asm__ volatile( "int $0x40" :: );
}

inline uint8_t lib_sys_int_return_bool() {
	// initialize local variable
	uint8_t rax;

	// call the kernel function
	__asm__ volatile(
		"int	$0x40"
		: "=a" (rax)
	);

	// return TRUE/FALSE
	return rax;
}

inline int64_t lib_sys_int_return_value() {
	// initialize local variable
	int64_t rax;

	// call the kernel function
	__asm__ volatile(
		"int	$0x40"
		: "=a" (rax)
	);

	// return value
	return rax;
}

inline uint64_t lib_sys_int_return_value_unsigned() {
	// initialize local variable
	uint64_t rax;

	// call the kernel function
	__asm__ volatile(
		"int	$0x40"
		: "=a" (rax)
	);

	// return unsigned value
	return rax;
}

inline uintptr_t lib_sys_int_return_pointer() {
	// initialize local variable
	uintptr_t rax;

	// call the kernel function
	__asm__ volatile(
		"int	$0x40"
		: "=a" (rax)
	);

	// return pointer
	return rax;
}
