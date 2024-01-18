#ifndef BRUSH_H
#define BRUSH_H

#include "raylib.h"

class Simulation;
class RenderCamera;

class Brush {
public:


private:
    
};


class BrushRenderer {
public:
    BrushRenderer(BrushRenderer &other) = delete;
    void operator=(const BrushRenderer&) = delete;
    ~BrushRenderer();

    static BrushRenderer * ref() {
        if (single == nullptr) [[unlikely]]
            single = new BrushRenderer;
        return single;
    };

    void draw();
    void update(Simulation * sim, RenderCamera * camera);
private:
    Brush currentBrush;
    int offset;
    unsigned int size;
    int x, y, z;    // Intersection point
    int bx, by, bz; // Actual brush pos 
    Vector2 prevMousePos;

    static BrushRenderer * single;

    void do_raycast(Simulation * sim, RenderCamera * camera);
    void do_controls(Simulation * sim);

    BrushRenderer():
        offset(0),
        size(5),
        x(-1), y(-1), z(-1) {}
};

#endif