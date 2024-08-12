/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

double alpha = 0.0f;

struct LIB_RGL_STRUCTURE *rgl;

// uint8_t json[] = "...";
MACRO_IMPORT_FILE_AS_STRING( json, "software/3d/interface.json" );

struct LIB_INTERFACE_STRUCTURE interface = { (uintptr_t) &json };

struct LIB_RGL_STRUCTURE_TRIANGLE *object;
uint64_t object_count = EMPTY;

double move_x = 0.0f;
double move_y = 0.0f;
double move_z = 2.0f;

double rotate_x = 0.0f;
double rotate_y = 0.0f;
double rotate_z = 0.0f;