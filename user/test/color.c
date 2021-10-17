#include "color.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

static bool use_color_detect(FILE *output)
{
	int is_tty = isatty(fileno(output));
	if (is_tty == 1) {
		return true;
	} else {
		errno = 0;
		return false;
	}
}

static bool use_color_default(FILE *output)
{
	const char *use_color = getenv("COLOR");
	if (!use_color || strcmp(use_color, "") == 0 ||
	    strcmp(use_color, "auto") == 0) {
		return use_color_detect(output);
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

ColorOutput ColorOutput_detect(FILE *output)
{
	return (ColorOutput){
		.output = output,
		.use_color = use_color_detect(output),
	};
}

ColorOutput ColorOutput_default(FILE *output)
{
	return (ColorOutput){
		.output = output,
		.use_color = use_color_default(output),
	};
}

void ColorOutput_set(ColorOutput self, const Color *colors)
{
	if (!self.use_color) {
		return;
	}
	if (*colors == Color_None) {
		return;
	}
	fprintf(self.output, "\x1b[%d", *colors);
	colors++;
	while (*colors != Color_None) {
		fprintf(self.output, ";%d", *colors);
		colors++;
	}
	fprintf(self.output, "m");
}

void ColorOutput_reset(ColorOutput self)
{
	ColorOutput_set(self, Colors(Color_Reset));
}

__attribute__((format(printf, 3, 4))) void
ColorOutput_printf(ColorOutput self, const Color *colors, const char *fmt, ...)
{
	ColorOutput_set(self, colors);
	va_list args = { 0 };
	va_start(args, fmt);
	assert(vfprintf(self.output, fmt, args) >= 0);
	va_end(args);
	ColorOutput_reset(self);
}
