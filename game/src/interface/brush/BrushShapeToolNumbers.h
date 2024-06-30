#ifndef INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_
#define INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_

#include "./tools/VolumeTool.h"
#include "./tools/FillTool.h"
#include "./tools/LineTool.h"
#include "./tools/BrushShapeTool.h"
#include "./tools/SignTool.h"
#include "./tools/SignMoveTool.h"

#include <array>

enum BRUSH_TOOL_NUMBERS {
    BRUSH_TOOL_DEFAULT,
    BRUSH_TOOL_VOLUME,
    BRUSH_TOOL_FILL,
    BRUSH_TOOL_LINE,
    BRUSH_TOOL_SIGN,
    BRUSH_TOOL_MOVE_SIGN,

    BRUSH_TOOL_LAST // DO NOT USE
};

extern std::array<BrushShapeTool*, (std::size_t)BRUSH_TOOL_LAST> BRUSH_TOOLS;

#endif // INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_
