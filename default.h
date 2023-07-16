/*=============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
=============================================================================*/

#ifndef	DEFAULT
	#define	DEFAULT

	#define	MACRO_DEBUF() { __asm__ volatile( "xchg %bx, %bx" ); }

	#define	MACRO_PAGE_ALIGN_UP( value )(((value) + STATIC_PAGE_SIZE_byte - 1) & ~(STATIC_PAGE_SIZE_byte - 1))
	#define	MACRO_PAGE_ALIGN_DOWN( value )((value) & ~(STATIC_PAGE_SIZE_byte - 1))

	#define	MACRO_BLOCK_ALIGN_UP( value, block )(((value) + block - 1) & ~(block - 1))

	#define	MACRO_ABS( N )(( N < 0 ) ? ( -N ) : ( N ))

	#define	MACRO_STR2( x ) #x
	#define	MACRO_STR( x ) MACRO_STR2( x )

	#define MACRO_IMPORT_FILE_AS_STRING( name, file ) \
		__asm__(	".section .rodata				\n" \
			".global " MACRO_STR( name ) "			\n" \
			".balign 16					\n" \
			MACRO_STR( name ) ":				\n" \
			".incbin \"" file "\"				\n" \
			".byte 0x00					\n" \
		); \
		extern const __attribute__( ( aligned( 16 ) ) ) void* name; \

	#define MACRO_IMPORT_FILE_AS_ARRAY( name, file ) \
		__asm__(	".section .rodata				\n" \
			".global file_" MACRO_STR( name ) "_start	\n" \
			".global file_" MACRO_STR( name ) "_end		\n" \
			".balign 16					\n" \
			"file_" MACRO_STR( name ) "_start:		\n" \
			".incbin \"" file "\"				\n" \
			"file_" MACRO_STR( name ) "_end:		\n" \
		); \
		extern const __attribute__( ( aligned( 16 ) ) ) void* file_ ## name ## _start; \
		extern const void* file_ ## name ## _end;

	#define MACRO_ENDIANNESS_WORD( value )( value >> STATIC_MOVE_HIGH_TO_AL_shift | value << STATIC_MOVE_AL_TO_HIGH_shift )
	#define MACRO_ENDIANNESS_DWORD( value )( value >> (STATIC_MOVE_HIGH_TO_AX_shift + STATIC_MOVE_HIGH_TO_AL_shift) | ((value >> STATIC_MOVE_HIGH_TO_AL_shift) & 0xFF00) | ((value << STATIC_MOVE_AL_TO_HIGH_shift) & 0xFF0000) | value << (STATIC_MOVE_AX_TO_HIGH_shift + STATIC_MOVE_AL_TO_HIGH_shift) )

	// definitions
	#include						"stdint.h"
	#include						"stddef.h"
	#include						"stdbool.h"
	#include						"stdarg.h"
	#include						"limine/limine.h"

	#define	EMPTY						0
	#define	ZERO						0

	#define	TRUE						1
	#define	FALSE						0

	#define	LOCK						TRUE
	#define	UNLOCK						FALSE

	#define STATIC_MAX_unsigned				-1

	#define	STATIC_VIDEO_DEPTH_shift			2
	#define	STATIC_VIDEO_DEPTH_byte				4
	#define	STATIC_VIDEO_DEPTH_bit				32

	#define	STATIC_PAGE_SIZE_page				1
	#define	STATIC_PAGE_SIZE_shift				12
	#define	STATIC_PAGE_SIZE_byte				4096
	#define	STATIC_PAGE_mask				0xFFFFFFFFFFFFF000

	#define	STATIC_DIVIDE_BY_2_shift			1
	#define	STATIC_DIVIDE_BY_WORD_shift			1
	#define	STATIC_DIVIDE_BY_4_shift			2
	#define	STATIC_DIVIDE_BY_DWORD_shift			2
	#define	STATIC_DIVIDE_BY_8_shift			3
	#define	STATIC_DIVIDE_BY_QWORD_shift			3
	#define	STATIC_DIVIDE_BY_16_shift			4
	#define	STATIC_DIVIDE_BY_32_shift			5
	#define	STATIC_DIVIDE_BY_64_shift			6
	#define	STATIC_DIVIDE_BY_PTR_shift			STATIC_DIVIDE_BY_64_shift
	#define	STATIC_DIVIDE_BY_512_shift			9
	#define	STATIC_DIVIDE_BY_1024_shift			10
	#define	STATIC_DIVIDE_BY_PAGE_shift			STATIC_PAGE_SIZE_shift

	#define	STATIC_MULTIPLE_BY_2_shift			1
	#define	STATIC_MULTIPLE_BY_WORD_shift			1
	#define	STATIC_MULTIPLE_BY_4_shift			2
	#define	STATIC_MULTIPLE_BY_DWORD_shift			2
	#define	STATIC_MULTIPLE_BY_8_shift			3
	#define	STATIC_MULTIPLE_BY_QWORD_shift			3
	#define	STATIC_MULTIPLE_BY_16_shift			4
	#define	STATIC_MULTIPLE_BY_64_shift			6
	#define	STATIC_MULTIPLE_BY_PTR_shift			STATIC_MULTIPLE_BY_64_shift
	#define	STATIC_MULTIPLE_BY_256_shift			8
	#define	STATIC_MULTIPLE_BY_512_shift			9
	#define	STATIC_MULTIPLE_BY_1024_shift			10
	#define	STATIC_MULTIPLE_BY_PAGE_shift			STATIC_PAGE_SIZE_shift

	#define	STATIC_WORD_SIZE_byte				2
	#define	STATIC_DWORD_SIZE_byte				4
	#define	STATIC_QWORD_SIZE_byte				8
	#define	STATIC_PTR_SIZE_byte				STATIC_QWORD_SIZE_byte

	#define	STATIC_BYTE_HALF_mask				0x000000000000000F
	#define	STATIC_WORD_mask				0x000000000000FFFF

	#define	STATIC_BYTE_HALF_SIZE_bit			4
	#define	STATIC_BYTE_SIZE_bit				8
	#define	STATIC_WORD_SIZE_bit				16
	#define	STATIC_DWORD_SIZE_bit				32
	#define	STATIC_QWORD_SIZE_bit				64
	#define	STATIC_PTR_SIZE_bit				STATIC_QWORD_SIZE_bit

	#define	STATIC_WORD_SIZE_shift				1

	#define	STATIC_QWORD_sign				0x8000000000000000

	#define	STATIC_BYTE_SIGN_bit				7
	#define	STATIC_WORD_SIGN_bit				15
	#define	STATIC_DWORD_SIGN_bit				31
	#define	STATIC_QWORD_SIGN_bit				63

	#define	STATIC_MOVE_AL_TO_HIGH_shift			8
	#define	STATIC_MOVE_AX_TO_HIGH_shift			16
	#define	STATIC_MOVE_EAX_TO_HIGH_shift			32
	#define	STATIC_MOVE_HIGH_TO_AL_shift			8
	#define	STATIC_MOVE_HIGH_TO_AX_shift			16
	#define	STATIC_MOVE_HIGH_TO_EAX_shift			32

	#define	STATIC_COLOR_ALPHA_mask				0xFF000000
	#define	STATIC_COLOR_black				0xFF000000
	#define	STATIC_COLOR_blue				0xFF1947B0
	#define	STATIC_COLOR_green				0xFF19B04F
	#define	STATIC_COLOR_cyan				0xFF7AC5CD
	#define	STATIC_COLOR_red				0xFFB01921
	#define	STATIC_COLOR_magenta				0xFFDF1FFF
	#define	STATIC_COLOR_violet				0xFFA300FF
	#define	STATIC_COLOR_gray_light				0xFFBFBFBF
	#define	STATIC_COLOR_gray				0xFF5D5D5D
	#define	STATIC_COLOR_blue_light				0xFF2668FF
	#define	STATIC_COLOR_green_light			0xFF26FF73
	#define	STATIC_COLOR_magenta_light			0xFFDC7CFF
	#define	STATIC_COLOR_red_light				0xFFFF2430
	#define	STATIC_COLOR_violet_light			0xFFA33EFF
	#define	STATIC_COLOR_yellow				0xFFD9E385
	#define	STATIC_COLOR_white				0xFFFEFEFE
	#define	STATIC_COLOR_BACKGROUND_default			STATIC_COLOR_black
	#define	STATIC_COLOR_default				STATIC_COLOR_gray_light

	#define	STATIC_ASCII_TERMINATOR				EMPTY
	#define	STATIC_ASCII_DIFFERENCE_0_A			0x07
	#define	STATIC_ASCII_BACKSPACE				0x08
	#define	STATIC_ASCII_TAB				0x09
	#define	STATIC_ASCII_NEW_LINE				0x0A
	#define	STATIC_ASCII_RETURN				0x0D
	#define	STATIC_ASCII_SPACE				0x20
	#define	STATIC_ASCII_EXCLAMATION			0x21
	#define	STATIC_ASCII_QUOTATION				0x22
	#define	STATIC_ASCII_DOLLAR				0x24
	#define STATIC_ASCII_PERCENT				0x25
	#define	STATIC_ASCII_AMPERSAND				0x26
	#define	STATIC_ASCII_ASTERISK				0x2A
	#define	STATIC_ASCII_PLUS				0x2B
	#define	STATIC_ASCII_COMMA				0x2C
	#define	STATIC_ASCII_MINUS				0x2D
	#define	STATIC_ASCII_DOT				0x2E
	#define	STATIC_ASCII_SLASH				0x2F
	#define	STATIC_ASCII_DIGIT_0				0x30
	#define	STATIC_ASCII_DIGIT_1				0x31
	#define	STATIC_ASCII_DIGIT_2				0x32
	#define	STATIC_ASCII_DIGIT_3				0x33
	#define	STATIC_ASCII_DIGIT_4				0x34
	#define	STATIC_ASCII_DIGIT_5				0x35
	#define	STATIC_ASCII_DIGIT_6				0x36
	#define	STATIC_ASCII_DIGIT_7				0x37
	#define	STATIC_ASCII_DIGIT_8				0x38
	#define	STATIC_ASCII_DIGIT_9				0x39
	#define STATIC_ASCII_COLON				0x3A
	#define STATIC_ASCII_LOWER_THAN				0x3C
	#define	STATIC_ASCII_GREATER_THAN			0x3E
	#define	STATIC_ASCII_QUESTION_MARK			0x3F
	#define	STATIC_ASCII_LETTER_A				0x41
	#define	STATIC_ASCII_LETTER_F				0x46
	#define	STATIC_ASCII_LETTER_N				0x4E
	#define	STATIC_ASCII_LETTER_T				0x54
	#define	STATIC_ASCII_LETTER_Z				0x5A
	#define	STATIC_ASCII_BRACKET_SQUARE_OPEN		0x5B
	#define	STATIC_ASCII_BACKSLASH				0x5C
	#define	STATIC_ASCII_BRACKET_SQUARE_CLOSE		0x5D
	#define	STATIC_ASCII_CARET				0x5E
	#define	STATIC_ASCII_LETTER_a				0x61
	#define	STATIC_ASCII_LETTER_b				0x62
	#define	STATIC_ASCII_LETTER_c				0x63
	#define	STATIC_ASCII_LETTER_d				0x64
	#define	STATIC_ASCII_LETTER_f				0x66
	#define	STATIC_ASCII_LETTER_h				0x68
	#define	STATIC_ASCII_LETTER_i				0x69
	#define	STATIC_ASCII_LETTER_l				0x6C
	#define	STATIC_ASCII_LETTER_n				0x6E
	#define	STATIC_ASCII_LETTER_q				0x71
	#define	STATIC_ASCII_LETTER_r				0x72
	#define	STATIC_ASCII_LETTER_s				0x73
	#define	STATIC_ASCII_LETTER_t				0x74
	#define	STATIC_ASCII_LETTER_u				0x75
	#define	STATIC_ASCII_LETTER_x				0x78
	#define	STATIC_ASCII_LETTER_z				0x7A
	#define	STATIC_ASCII_BRACLET_OPEN			0x7B
	#define	STATIC_ASCII_STREAM				0x7C
	#define	STATIC_ASCII_BRACLET_CLOSE			0x7D
	#define	STATIC_ASCII_TILDE				0x7E
	#define	STATIC_ASCII_DELETE				0x7F

	#define	STATIC_SEQUENCE_LENGTH				0x04

	#define	STATIC_SEQUENCE_CLEAR				"^[t00"			// wyczyść przestrzeń konsoli/terminala
	#define	STATIC_SEQUENCE_CLEAR_LINE			"^[t01"			// wyczyść aktualną linię
	#define	STATIC_SEQUENCE_CLEAR_ENABLE			"^[t02"
	#define	STATIC_SEQUENCE_CLEAR_DISABLE			"^[t03"
	#define	STATIC_SEQUENCE_CURSOR_RESET			"^[t20"		// position 0,0
	#define	STATIC_SEQUENCE_CURSOR_ENABLE			"^[t21"		// włącz kursor tekstowy
	#define	STATIC_SEQUENCE_CURSOR_DISABLE			"^[t22"		// wyłącz kursor tekstowy
	#define	STATIC_SEQUENCE_CURSOR_PUSH			"^[t23"		// zapamiętaj pozycję
	#define	STATIC_SEQUENCE_CURSOR_POP			"^[t24"		// przywróć pozycję
	#define	STATIC_SEQUENCE_CURSOR_UP			"^[t25"		// przesuń kursor o pozycję w górę
	#define	STATIC_SEQUENCE_CURSOR_DOWN			"^[t26"		// przesuń kursor o pozycję w dół
	#define	STATIC_SEQUENCE_CURSOR_LEFT			"^[t27"		// przesuń kursor o pozycję w lewo
	#define	STATIC_SEQUENCE_CURSOR_RIGHT			"^[t28"		// przesuń kursor o pozycję w prawo

	#define	STATIC_SEQUENCE_COLOR_DEFAULT			"^[c07"			// kolor jasno-szary na czarnym tle

	#define	STATIC_SEQUENCE_COLOR_BLACK			"^[c*0"
	#define	STATIC_SEQUENCE_COLOR_BLUE			"^[c*1"
	#define	STATIC_SEQUENCE_COLOR_GREEN			"^[c*2"
	#define	STATIC_SEQUENCE_COLOR_CYAN			"^[c*3"
	#define	STATIC_SEQUENCE_COLOR_RED			"^[c*4"
	#define	STATIC_SEQUENCE_COLOR_MAGENTA			"^[c*5"
	#define	STATIC_SEQUENCE_COLOR_VIOLET			"^[c*6"
	#define	STATIC_SEQUENCE_COLOR_GRAY_LIGHT		"^[c*7"
	#define	STATIC_SEQUENCE_COLOR_GRAY			"^[c*8"
	#define	STATIC_SEQUENCE_COLOR_BLUE_LIGHT		"^[c*9"
	#define	STATIC_SEQUENCE_COLOR_GREEN_LIGHT		"^[c*A"
	#define	STATIC_SEQUENCE_COLOR_BROWN			"^[c*B"
	#define	STATIC_SEQUENCE_COLOR_RED_LIGHT			"^[c*C"
	#define	STATIC_SEQUENCE_COLOR_VIOLET_LIGHT		"^[c*D"
	#define	STATIC_SEQUENCE_COLOR_YELLOW			"^[c*E"
	#define	STATIC_SEQUENCE_COLOR_WHITE			"^[c*F"

	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_BLACK		"^[c0*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_BLUE		"^[c1*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_GREEN		"^[c2*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_CYAN		"^[c3*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_RED		"^[c4*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_MAGENTA	"^[c5*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_BROWN_LIGHT	"^[c6*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_GRAY_LIGHT	"^[c7*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_GRAY		"^[c8*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_BLUE_LIGHT	"^[c9*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_GREEN_LIGHT	"^[cA*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_BROWN		"^[cB*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_RED_LIGHT	"^[cC*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_ORANGE		"^[cD*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_YELLOW		"^[cE*"
	#define	STATIC_SEQUENCE_COLOR_BACKGROUND_WHITE		"^[cF*"

	#define	STATIC_NUMBER_SYSTEM_binary			2
	#define STATIC_NUMBER_SYSTEM_octal			8
	#define	STATIC_NUMBER_SYSTEM_decimal			10
	#define STATIC_NUMBER_SYSTEM_hexadecimal		16
#endif
