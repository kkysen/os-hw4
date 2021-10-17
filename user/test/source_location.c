#include "source_location.h"

void SourceLocation_print(SourceLocation self, ColorOutput output)
{
	ColorOutput_printf(output, Colors(Color_FG_Magenta, Color_Bright), "[");
	ColorOutput_printf(output,
			   Colors(Color_FG_Blue, Color_Bright, Color_Underline),
			   "%s", self.file);
	ColorOutput_printf(output, Colors(Color_Bright), ":");
	ColorOutput_printf(output, Colors(Color_FG_Green, Color_Bright), "%u",
			   self.line);
	ColorOutput_printf(output, Colors(Color_Bright), ":");
	ColorOutput_printf(output, Colors(Color_FG_Yellow, Color_Bright), "%s",
			   self.func);
	ColorOutput_printf(output, Colors(Color_FG_Magenta, Color_Bright), "]");
}
