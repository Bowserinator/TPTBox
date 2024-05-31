#ifndef BRUSH_H
#define BRUSH_H

#include "raylib.h"
#include "../../util/vector_op.h"
#include "Brushes.h"

class Simulation;
class RenderCamera;

class BrushRenderer {
public:
    BrushRenderer(Simulation * sim, RenderCamera * camera):
        offset(0),
        size(5),
        x(-1), y(-1), z(-1),
        selected_element(1),
        sim(sim), camera(camera) {}
    BrushRenderer(BrushRenderer &other) = delete;

    std::string tooltip_to_display = "";

    void draw();
    void update();

    void set_selected_element(int element) { selected_element = element; tool_mode = false; }
    void set_selected_tool(int tool) { selected_tool = tool; tool_mode = true; }
    void set_misc_data(int data) { misc_data = data; }

    bool brush_in_sim() const { return x >= 0 && y >= 0 && z >= 0; }
    Vector3T<int> get_raycast_pos() const { return Vector3T<int>{ x, y, z }; };
    Vector3T<int> get_brush_pos() const { return Vector3T<int>{ bx, by, bz }; };
    int get_offset() const { return offset; }
    unsigned int get_size() const { return size; }
private:
    std::size_t currentBrushIdx = 1;
    int offset;
    unsigned int size;
    int x, y, z;    // Intersection point
    int bx, by, bz; // Actual brush pos

    int selected_element;
    int selected_tool;
    int misc_data;
    bool tool_mode = false;

    Vector2 prevMousePos;
    Vector3 prevCameraPos;
    unsigned int prevSimFrameCount = -1;

    Simulation * sim;
    RenderCamera * camera;

    void do_raycast(Simulation * sim, RenderCamera * camera);
    void do_controls(Simulation * sim);
};

#endif