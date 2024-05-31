#ifndef GRAPHICS_SETTING_DATA_H
#define GRAPHICS_SETTING_DATA_H

#include "AbstractSettingsData.h"
#include "../../../render/Renderer.h"

namespace settings {
    class Graphics : public AbstractSettingsData {
    public:
        Graphics() {}
        Graphics(Graphics&other) = delete;
        void operator=(const Graphics&) = delete;
        ~Graphics() = default;

        virtual void loadFromJSON(const nlohmann::json &json) {
            if (json.contains("graphics")) {
                aoStrength = util::clamp(json["graphics"].value("aoStrength", aoStrength), 0.0f, 1.0f);
                shadowStrength = util::clamp(json["graphics"].value("shadowStrength", shadowStrength), 0.0f, 1.0f);

                showOutline = json["graphics"].value("showOutline", showOutline);
                enableTransparency = json["graphics"].value("enableTransparency", enableTransparency);
                enableReflection = json["graphics"].value("enableReflection", enableReflection);
                enableRefraction = json["graphics"].value("enableRefraction", enableRefraction);
                enableBlur = json["graphics"].value("enableBlur", enableBlur);
                enableGlow = json["graphics"].value("enableGlow", enableGlow);
                enableAO = json["graphics"].value("enableAO", enableAO);
                enableShadows = json["graphics"].value("enableShadows", enableShadows);
                fullScreen = json["graphics"].value("fullScreen", fullScreen);
                heatViewMin = json["graphics"].value("heatViewMin", heatViewMin);
                heatViewMax = json["graphics"].value("heatViewMax", heatViewMax);
                renderDownscale = json["graphics"].value("renderDownscale", renderDownscale);
                backgroundColor = GetColor(json["graphics"].value("backgroundColor", (unsigned int)ColorToInt(backgroundColor)));
                shadowColor = GetColor(json["graphics"].value("shadowColor", (unsigned int)ColorToInt(shadowColor)));
            }
        }

        virtual void writeToJSON(nlohmann::json &json) const {
            json["graphics"]["showOutline"] = showOutline;
            json["graphics"]["aoStrength"] = aoStrength;
            json["graphics"]["shadowStrength"] = shadowStrength;
            json["graphics"]["enableTransparency"] = enableTransparency;
            json["graphics"]["enableReflection"] = enableReflection;
            json["graphics"]["enableRefraction"] = enableRefraction;
            json["graphics"]["enableBlur"] = enableBlur;
            json["graphics"]["enableGlow"] = enableGlow;
            json["graphics"]["enableAO"] = enableAO;
            json["graphics"]["enableShadows"] = enableShadows;
            json["graphics"]["fullScreen"] = fullScreen;
            json["graphics"]["heatViewMin"] = heatViewMin;
            json["graphics"]["heatViewMax"] = heatViewMax;
            json["graphics"]["renderDownscale"] = renderDownscale;
            json["graphics"]["backgroundColor"] = (unsigned int)ColorToInt(backgroundColor);
            json["graphics"]["shadowColor"] = (unsigned int)ColorToInt(shadowColor);
        }

        Renderer::FragDebugMode renderMode = Renderer::FragDebugMode::NODEBUG;

        static constexpr float defaultHeatViewMax = 5000.0f;
        static constexpr float defaultRenderDownscale = 1.5f;
        static constexpr float defaultAOStrength = 0.6f;
        static constexpr float defaultShadowStrength = 0.35f;
        static constexpr Color defaultBackgroundColor = { 0, 0, 0, 255 };
        static constexpr Color defaultShadowColor = { 32, 18, 39, 255 };

        float aoStrength = defaultAOStrength;
        float shadowStrength = defaultShadowStrength;
        bool showOctree = false;
        bool showOutline = false;
        float heatViewMin = MIN_TEMP;
        float heatViewMax = defaultHeatViewMax;
        float renderDownscale = defaultRenderDownscale;
        Color backgroundColor = defaultBackgroundColor;
        Color shadowColor = defaultShadowColor;

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