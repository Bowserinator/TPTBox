#ifndef INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_
#define INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_

#include "raylib.h"
#include "../../../util/vector_op.h"
#include "../../../simulation/Simulation.h"
#include "../../hud/abstract/IMiddleTooltip.h"
#include "../../settings/data/SettingsData.h"

#include "../Brushes.h"
#include "../BrushMesher.h"
#include "../Brush.h"

#include <algorithm>
#include <string>
#include <vector>
#include <functional>

#define CLICK_BRUSH_OP_PARAMS const std::vector<Vector3T<int>> &points, BrushRenderer * brush_renderer, Simulation * sim, bool is_filling_element

/** A tool that works by clicking on two points */
class BrushShapeTool {
public:
    explicit BrushShapeTool(const std::string &name, const unsigned int points_required):
        name(name), points_required(points_required) {};

    const std::string name;
    const unsigned int points_required;

    /**
     * @brief Apply brush operation
     * @param start Start location, start.xyz <= end.xyz, in sim bounds
     * @param end   End location, in sim bounds
     * @param brush_renderer
     */
    virtual void operation(CLICK_BRUSH_OP_PARAMS) = 0;
};

#endif // INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_
