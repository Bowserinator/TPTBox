#ifndef INTERFACE_GUI_SCENE_H_
#define INTERFACE_GUI_SCENE_H_

#include <vector>
#include <algorithm>
#include <iostream>

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
            child->setParentScene(this);
        }

        // Remove a child
        // @param child - Child pointer
        void removeChild(Component * component);

        // Remove child, but don't free memory
        // @param child - Child ptr
        void removeChildDontDelete(Component * component);

        // Called after OpenGL context is initialized
        virtual void init() {};

        // Called every frame, ticks all child components
        virtual void update();

        // Draw the scene
        virtual void draw();

        // Process children deletion
        static void processChildrenDeletions(std::vector<Component *> &children) {
            auto itr = std::partition(children.begin(), children.end(), [](auto c) { return !c->toBeDeleted(); });
            for (auto i = itr; i != children.end(); ++i)
                delete *i;
            if (itr != children.end())
                children.erase(itr, children.end());
            for (auto child : children)
                child->processDeletion();
        }

    private:
        std::vector<Component *> children;
        Vector2 pos;
    };
} // namespace ui

#endif // INTERFACE_GUI_SCENE_H_
