#pragma once

#include "prelude.h"

#include <stdio.h>

typedef enum {
	Color_None = -1,
	Color_Reset = 0,
	Color_Bright = 1,
	Color_Dim = 2,
	Color_Underline = 4,
	Color_Blink = 5,
	Color_Reverse = 7,
	Color_Hidden = 8,
	Color_FG_Black = 30,
	Color_FG_Red = 31,
	Color_FG_Green = 32,
	Color_FG_Yellow = 33,
	Color_FG_Blue = 34,
	Color_FG_Magenta = 35,
	Color_FG_Cyan = 36,
	Color_FG_White = 37,
	Color_BG_Black = 40,
	Color_BG_Red = 41,
	Color_BG_Green = 42,
	Color_BG_Yellow = 43,
	Color_BG_Blue = 44,
	Color_BG_Magenta = 45,
	Color_BG_Cyan = 46,
	Color_BG_White = 47,
} Color;

typedef struct {
	FILE *output;
	bool use_color;
} ColorOutput;

ColorOutput ColorOutput_detect(FILE *output);

ColorOutput ColorOutput_default(FILE *output);

/// `colors` must be an array terminated by `Color_None`.
/// See the `Colors` macros.
void ColorOutput_set(ColorOutput self, const Color *colors);

#define Colors(...) ((Color[]){ __VA_ARGS__, Color_None })

void ColorOutput_reset(ColorOutput self);

__attribute__((format(printf, 3, 4))) void
ColorOutput_printf(ColorOutput self, const Color *colors, const char *fmt, ...);
