#ifndef INTERFACE_BRUSH_TOOLS_FILLTOOL_H_
#define INTERFACE_BRUSH_TOOLS_FILLTOOL_H_

#include "BrushShapeTool.h"
#include "../../../simulation/SimulationDef.h"

#include <vector>
#include <queue>

class FillBrushTool : public BrushShapeTool {
public:
    FillBrushTool(): BrushShapeTool("Fill", 1) {
        visited = std::vector<bool>(XRES * YRES * ZRES, false);
    }

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        std::fill(visited.begin(), visited.end(), false);
        auto idx = [](int x, int y, int z) { return z * XRES * YRES + y * XRES + x; };
        auto type_at = [sim](Vector3T<coord_t> coord) {
            pmap_id at_id = sim->pmap[coord.z][coord.y][coord.x];
            if (!at_id)
                at_id = sim->photons[coord.z][coord.y][coord.x];
            return TYP(at_id);
        };

        std::queue<Vector3T<coord_t>> to_visit;
        auto &start = points[0];
        to_visit.push(Vector3T<coord_t>(start.x, start.y, start.z));

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
            Vector3T<coord_t> coord = to_visit.front();
            to_visit.pop();

            if (visited[idx(coord.x, coord.y, coord.z)])
                continue;
            visited[idx(coord.x, coord.y, coord.z)] = true;
            if (type_at(coord) != type_check)
                continue;

            brush_renderer->apply_brush_op(coord.x, coord.y, coord.z);

            #define NEIGHBOR(dx, dy, dz) if (BOUNDS_CHECK(coord.x + dx, coord.y + dy, coord.z + dz) && \
                        settings::data::ref()->graphics->in_view_slice(coord.x + dx, coord.y + dy, coord.z + dz) && \
                        !visited[idx(coord.x + dx, coord.y + dy, coord.z + dz)]) \
                    to_visit.push(Vector3T<coord_t>(coord.x + dx, coord.y + dy, coord.z + dz));

            // x scan line
            for (int i = 0; i < 2; i++) {
                int dx = i == 0 ? 1 : -1;
                while (BOUNDS_CHECK(coord.x + dx, coord.y, coord.z) &&
                        settings::data::ref()->graphics->in_view_slice(coord.x + dx, coord.y, coord.z) &&
                        !visited[idx(coord.x + dx, coord.y, coord.z)]) {
                    if (type_at(Vector3T<coord_t>(coord.x + dx, coord.y, coord.z)) != type_check)
                        break;

                    brush_renderer->apply_brush_op(coord.x + dx, coord.y, coord.z);
                    visited[idx(coord.x + dx, coord.y, coord.z)] = true;

                    NEIGHBOR(dx, -1, 0);
                    NEIGHBOR(dx, 1, 0);
                    NEIGHBOR(dx, 0, -1);
                    NEIGHBOR(dx, 0, 1);
                    dx += i == 0 ? 1 : -1;
                }
            }

            NEIGHBOR(0, -1, 0);
            NEIGHBOR(0, 1, 0);
            NEIGHBOR(0, 0, -1);
            NEIGHBOR(0, 0, 1);
        }
    }

private:
    std::vector<bool> visited;
};

#endif // INTERFACE_BRUSH_TOOLS_FILLTOOL_H_
