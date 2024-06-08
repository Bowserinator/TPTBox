#ifndef INTERFACE_BRUSH_BRUSH_H_
#define INTERFACE_BRUSH_BRUSH_H_

#include "raylib.h"
#include "../../util/vector_op.h"
#include "../../simulation/Simulation.h"
#include "../hud/abstract/IMiddleTooltip.h"

#include "Brushes.h"
#include "BrushMesher.h"

#include <algorithm>
#include <string>
#include <vector>
#include <functional>

constexpr unsigned int MAX_BRUSH_SIZE = std::max(std::max(2 * XRES, 2 * YRES), 2 * ZRES);
constexpr float MIN_BRUSH_REMESH_DELAY_SECONDS = 0.03;

class Simulation;
class RenderCamera;
class Renderer;

class BrushRenderer : public IMiddleTooltip {
public:
    BrushRenderer(Simulation * sim, RenderCamera * camera):
        offset(0),
        size(5, 5, 5),
        x(-1), y(-1), z(-1),
        selected_element(1),
        sim(sim), camera(camera) {}
    BrushRenderer(const BrushRenderer &other) = delete;

    void draw(Renderer * renderer);
    void update();

    void set_selected_element(int element) { selected_element = element; tool_mode = false; }
    void set_selected_tool(int tool) { selected_tool = tool; tool_mode = true; }
    void set_misc_data(int data) { misc_data = data; }

    bool brush_in_sim() const { return x >= 0 && y >= 0 && z >= 0; }
    Vector3T<int> get_raycast_pos() const { return Vector3T<int>{ x, y, z }; };
    Vector3T<int> get_brush_pos() const { return Vector3T<int>{ bx, by, bz }; };
    int get_offset() const { return offset; }
    Vector3T<unsigned int> get_size() const { return size; }
    std::size_t get_brush_type() const { return currentBrushIdx; }

    void add_change_listener(std::function<void()> f) { change_callbacks.push_back(f); }
    void set_brush_type(std::size_t currentBrushIdx);
    void set_size(Vector3T<unsigned int> size) { this->size = size; }

private:
    std::vector<std::function<void()>> change_callbacks;

    int offset;
    Vector3T<unsigned int> size;
    int x, y, z;    // Intersection point
    int bx, by, bz; // Actual brush pos
    float last_remesh_time = 0.0f;

    int selected_element;
    int selected_tool;
    int misc_data;
    bool tool_mode = false;

    Vector2 prevMousePos;
    Vector3 prevCameraPos;
    unsigned int prevSimFrameCount = -1;

    Simulation * sim;
    RenderCamera * camera;
    RaycastOutput raycast_out;

    std::size_t currentBrushIdx = 1;
    std::size_t previousBrushIdx = INT_MAX;
    Vector3T<unsigned int> previousSize{ INT_MAX, INT_MAX, INT_MAX };
    BrushFaceModels current_brush_mesh;

    void do_raycast(Simulation * sim, RenderCamera * camera);
    void do_controls(Simulation * sim);
    void update_offset();
};

#endif // INTERFACE_BRUSH_BRUSH_H_
