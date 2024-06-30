#ifndef INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_
#define INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_

#include "raylib.h"
#include "../../../util/vector_op.h"
#include "../../../simulation/Simulation.h"
#include "../../hud/abstract/IMiddleTooltip.h"
#include "../../settings/data/SettingsData.h"

#include "../../../render/Renderer.h"
#include "../../../render/camera/camera.h"
#include "../../../util/graphics.h"

#include "../Brushes.h"
#include "../Preview.h"
#include "../Brush.h"

#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <functional>

#define CLICK_BRUSH_OP_PARAMS const std::vector<Vector3T<int>> &points, BrushRenderer * brush_renderer, Simulation * sim, bool is_filling_element
#define CLICK_BRUSH_DRAW_PARAMS const std::vector<Vector3T<int>> &points, Renderer * renderer, BrushRenderer * brush_renderer, const Vector3T<int> pos
#define CLICK_BRUSH_REMESH_PARAMS const std::vector<Vector3T<int>> &points, Renderer * renderer, BrushRenderer * brush_renderer, const Vector3T<int> pos
#define CANCEL_BRUSH_PARAMS BrushRenderer * brush_renderer, Simulation * sim

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

    /** Called when right click clears all points */
    virtual void onCancel(CANCEL_BRUSH_PARAMS) { }

    /**
     * @brief Draw the tool preview
     */
    virtual void draw(CLICK_BRUSH_DRAW_PARAMS) = 0;

    /**
     * @brief Called when size or position changes, should update preview_models
     */
    virtual void remesh(CLICK_BRUSH_REMESH_PARAMS) = 0;

protected:
    std::array<Model, 3> preview_models = {0};

    void unload_models() {
        for (auto i = 0; i < preview_models.size(); i++)
            UnloadModel(preview_models[i]);
        preview_models = {0};
    }
};

#endif // INTERFACE_BRUSH_TOOLS_BRUSHSHAPETOOL_H_
