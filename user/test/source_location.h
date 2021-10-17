#pragma once

#include "prelude.h"

#include "color.h"

typedef struct {
	u32 line;
	const char *file;
	const char *func;
} SourceLocation;

#define HERE                                                                   \
	((SourceLocation){                                                     \
		.line = __LINE__,                                              \
		.file = __FILE__,                                              \
		.func = __func__,                                              \
	})

void SourceLocation_print(SourceLocation self, ColorOutput output);
