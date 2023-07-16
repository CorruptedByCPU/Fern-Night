/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	LIB_VFS
	#define	LIB_VFS

	#define	LIB_VFS_align			16
	#define	LIB_VFS_base			64
	#define	LIB_VFS_length			4
	#define	LIB_VFS_magic			0x53465623	// "#VFS"
	#define	LIB_VFS_name_limit		42
	#define	LIB_VFS_shift			6
	#define	LIB_VFS_default			2

	#define	LIB_VFS_TYPE_regular_file	0b00000001
	#define	LIB_VFS_TYPE_directory		0b00000010
	#define	LIB_VFS_TYPE_symbolic_link	0b00000100
	#define	LIB_VFS_TYPE_shared_object	0b00001000

	#define	LIB_VFS_MODE_user_executable	0x0000000000000001

	// this structure should be divisible by power of 2
	struct LIB_VFS_STRUCTURE {
		uint64_t	offset;
		uint64_t	size;
		uint16_t	length;
		uint16_t	mode;
		uint8_t		type;
		uint8_t		name[ LIB_VFS_name_limit + 1 ];	// last is STATIC_ASCII_TERMINATOR
	} __attribute__( (packed) );

	uint8_t lib_vfs_check( uintptr_t address, uint64_t size_byte );
	struct KERNEL_STORAGE_STRUCTURE_FILE lib_vfs_file( struct LIB_VFS_STRUCTURE *vfs, uint8_t *name, uint64_t length );
	void lib_vfs_init( struct LIB_VFS_STRUCTURE *vfs );
	void lib_vfs_read( struct LIB_VFS_STRUCTURE *knot, uintptr_t destination );
#endif