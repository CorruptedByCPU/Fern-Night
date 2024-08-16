/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_IPC_STRUCTURE shell_message	= { EMPTY };

uint8_t shell_command[ SHELL_COMMAND_limit ]	= { EMPTY };

uint8_t	shell_key_alt_left			= FALSE;
uint8_t	shell_key_ctrl				= FALSE;

uint8_t shell_exec_clear[]			= "clear";
uint8_t shell_exec_exit[]			= "exit";

uint64_t shell_exec_pid				= EMPTY;