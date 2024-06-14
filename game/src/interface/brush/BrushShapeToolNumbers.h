#ifndef INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_
#define INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_

#include "./tools/VolumeTool.h"
#include "./tools/FillTool.h"
#include "./tools/BrushShapeTool.h"

#include <array>

enum BRUSH_TOOL_NUMBERS {
    BRUSH_TOOL_DEFAULT,
    BRUSH_TOOL_VOLUME,
    BRUSH_TOOL_FILL,

    BRUSH_TOOL_LAST // DO NOT USE
};

extern std::array<BrushShapeTool*, (std::size_t)BRUSH_TOOL_LAST> BRUSH_TOOLS;

#endif // INTERFACE_BRUSH_BRUSHSHAPETOOLNUMBERS_H_
