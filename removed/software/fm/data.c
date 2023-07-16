/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

MACRO_IMPORT_FILE_AS_STRING( json, "software/fm/interface.json" );
struct LIB_INTERFACE_STRUCTURE interface = { (uintptr_t) &json };