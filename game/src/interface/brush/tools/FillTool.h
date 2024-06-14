#ifndef INTERFACE_BRUSH_TOOLS_FILLTOOL_H_
#define INTERFACE_BRUSH_TOOLS_FILLTOOL_H_

#include "BrushShapeTool.h"

#include <vector>
#include <queue>

/** A tool that works by clicking on two points */
class FillBrushTool : public BrushShapeTool {
public:
    FillBrushTool(): BrushShapeTool("Fill") {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        std::vector<bool> visited(XRES * YRES * ZRES, false);
        auto idx = [](int x, int y, int z) { return z * XRES * YRES + y * XRES + x; };
        std::queue<Vector3T<int>> to_visit;
        to_visit.push(start);

        // If placing elements, can't fill if start location is occupied
        pmap_id start_id = sim->pmap[start.z][start.y][start.x];
        if (!start_id)
            start_id = sim->photons[start.z][start.y][start.x];
        if (is_filling_element && start_id)
            return;
        const part_type type_check = TYP(start_id);

        // Deleting, but filling nothing so we delete nothing
        if (type_check == 0 && brush_renderer->is_delete_mode())
            return;

        while (to_visit.size()) {
            Vector3T<int> coord = to_visit.front();
            to_visit.pop();

            if (visited[idx(coord.x, coord.y, coord.z)])
                continue;
            visited[idx(coord.x, coord.y, coord.z)] = true;

            pmap_id at_id = sim->pmap[coord.z][coord.y][coord.x];
            if (!at_id)
                at_id = sim->photons[coord.z][coord.y][coord.x];
            if (TYP(at_id) != type_check)
                continue;

            brush_renderer->apply_brush_op(coord.x, coord.y, coord.z);

            #define NEIGHBOR(dx, dy, dz) if (BOUNDS_CHECK(coord.x + dx, coord.y + dy, coord.z + dz) && \
                        settings::data::ref()->graphics->in_view_slice(coord.x + dx, coord.y + dy, coord.z + dz) && \
                        !visited[idx(coord.x + dx, coord.y + dy, coord.z + dz)]) \
                    to_visit.push(Vector3T<int>(coord.x + dx, coord.y + dy, coord.z + dz));

            NEIGHBOR(-1, 0, 0);
            NEIGHBOR(1, 0, 0);
            NEIGHBOR(0, -1, 0);
            NEIGHBOR(0, 1, 0);
            NEIGHBOR(0, 0, -1);
            NEIGHBOR(0, 0, 1);
        }
    }
};

#endif // INTERFACE_BRUSH_TOOLS_FILLTOOL_H_
