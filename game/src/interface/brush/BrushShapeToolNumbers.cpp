#include "BrushShapeToolNumbers.h"

std::array<BrushShapeTool*, (std::size_t)BRUSH_TOOL_LAST> BRUSH_TOOLS{
    nullptr,               // BRUSH_TOOL_DEFAULT
    new VolumeBrushTool(), // BRUSH_TOOL_VOLUME
    new FillBrushTool(),   // BRUSH_TOOL_FILL
    new LineBrushTool(),   // BRUSH_TOOL_LINE
    new SignBrushTool(),   // BRUSH_TOOL_SIGN
    new SignMoveBrushTool(), // BRUSH_TOOL_MOVE_SIGN
};
