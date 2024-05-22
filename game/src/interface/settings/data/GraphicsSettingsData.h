#ifndef GRAPHICS_SETTING_DATA_H
#define GRAPHICS_SETTING_DATA_H

#include "../../../render/Renderer.h"

namespace settings {
    class Graphics {
    public:
        Graphics() {}
        Graphics(Graphics&other) = delete;
        void operator=(const Graphics&) = delete;
        ~Graphics() = default;

        Renderer::FragDebugMode renderMode = Renderer::FragDebugMode::NODEBUG;
        float aoStrength = 0.6f;
        float shadowStrength = 0.35f;
        bool showOctree = false;

        static constexpr float defaultAOStrength = 0.6f;
        static constexpr float defaultShadowStrength = 0.35f;

        bool enableTransparency = true;
        bool enableReflection = true;
        bool enableRefraction = true;
        bool enableBlur = true;
        bool enableGlow = true;
        bool enableAO = true;
        bool enableShadows = true;

        bool fullScreen = false;
    };
}

#endif