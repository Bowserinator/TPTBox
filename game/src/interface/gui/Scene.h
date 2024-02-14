#ifndef GUI_SCENE_H
#define GUI_SCENE_H

#include <vector>
#include "raylib.h"
#include "./components/abstract/Component.h"

// A global level container for all components
namespace ui {
    class Scene {
    public:
        Scene(const Vector2 pos = Vector2{0, 0}): pos(pos) {}
        virtual ~Scene();

        // Add a child. Parent is not assigned since top level
        // @param child - New child, must be heap allocated
        void addChild(Component * child) {
            children.push_back(child);
        }

        // Called after OpenGL context is initialized
        virtual void init() {};

        // Called every frame, ticks all child components
        virtual void update();

        // Draw the scene
        virtual void draw();
    private:
        std::vector<Component *> children;
        Vector2 pos;
    };
}

#endif