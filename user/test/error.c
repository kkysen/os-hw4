#include "error.h"

#include <stdarg.h>
#include <assert.h>
#include <string.h>

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

static bool Color_use_auto(FILE *output)
{
	const int is_tty = isatty(fileno(output));
	if (is_tty == 1) {
		return true;
	} else {
		errno = 0;
		return false;
	}
}

static bool Color_use(FILE *output)
{
	const char *use_color = getenv("COLOR");
	if (!use_color || strcmp(use_color, "") == 0 ||
	    strcmp(use_color, "auto") == 0) {
		return Color_use_auto(output);
	} else if (strcmp(use_color, "always") == 0) {
		return true;
	} else if (strcmp(use_color, "never") == 0) {
		return false;
	} else {
		fprintf(stderr,
			"$COLOR must be one of auto, always, never but is %s\n",
			use_color);
		abort();
	}
}

// colors terminated by Color_None
static void Color_set(FILE *output, bool use_color, const Color *colors)
{
	if (!use_color) {
		return;
	}
	if (*colors == Color_None) {
		return;
	}
	fprintf(output, "\x1b[%d", *colors);
	colors++;
	while (*colors != Color_None) {
		fprintf(output, ";%d", *colors);
		colors++;
	}
	fprintf(output, "m");
}

#define Colors(...) ((Color[]){ __VA_ARGS__, Color_None })

static void Color_reset(FILE *output, bool use_color)
{
	Color_set(output, use_color, Colors(Color_Reset));
}

__attribute__((format(printf, 4, 5))) static void
Color_printf(FILE *output, bool use_color, const Color *colors, const char *fmt,
	     ...)
{
	Color_set(output, use_color, colors);
	va_list args = { 0 };
	va_start(args, fmt);
	assert(vfprintf(output, fmt, args) >= 0);
	va_end(args);
	Color_reset(output, use_color);
}

void SourceLocation_print(SourceLocation self, FILE *output, bool use_color)
{
	Color_printf(output, use_color, Colors(Color_FG_Red, Color_Bright),
		     "[");
	Color_printf(output, use_color,
		     Colors(Color_FG_Blue, Color_Bright, Color_Underline), "%s",
		     self.file);
	Color_printf(output, use_color, Colors(Color_Bright), ":");
	Color_printf(output, use_color, Colors(Color_FG_Green, Color_Bright),
		     "%u", self.line);
	Color_printf(output, use_color, Colors(Color_Bright), ":");
	Color_printf(output, use_color, Colors(Color_FG_Yellow, Color_Bright),
		     "%s", self.func);
	Color_printf(output, use_color, Colors(Color_FG_Red, Color_Bright),
		     "]");
}

void check_eq(FILE *output, SourceLocation location, const void *actual,
	      const void *expected, bool (*eq)(const void *, const void *),
	      void (*print)(FILE *, const void *), const char *prefix)
{
	if (eq(actual, expected)) {
		return;
	}

	bool use_color = Color_use(output);

	SourceLocation_print(location, output, use_color);
	fprintf(output, " ");
	if (prefix) {
		fprintf(output, "%s: ", prefix);
	}
	Color_printf(output, use_color, Colors(Color_Bright), "expected");
	fprintf(output, " ");
	Color_set(output, use_color, Colors(Color_FG_Green, Color_Bright));
	print(output, expected);
	Color_reset(output, use_color);
	fprintf(output, " but ");
	Color_printf(output, use_color, Colors(Color_Bright), "found");
	fprintf(output, " ");
	Color_set(output, use_color, Colors(Color_FG_Red, Color_Bright));
	print(output, actual);
	Color_reset(output, use_color);
	fprintf(output, "\n");
	fflush(output);
}

static bool errno_eq(const void *a_void, const void *b_void)
{
	const int *a = a_void;
	const int *b = b_void;
	return *a == *b;
}

static void errno_print(FILE *output, const void *e_void)
{
	const int *e = e_void;
	fprintf(output, "%s", errno_name(*e));
}

void check_errno(FILE *output, SourceLocation location, int error)
{
	int expected = error;
	int actual = errno;
	errno = 0;
	check_eq(output, location, &actual, &expected, errno_eq, errno_print,
		 NULL);
}
