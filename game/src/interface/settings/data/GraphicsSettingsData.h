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
            }
        }

        virtual void writeToJSON(nlohmann::json &json) const {
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
        }

        Renderer::FragDebugMode renderMode = Renderer::FragDebugMode::NODEBUG;
        float aoStrength = 0.6f;
        float shadowStrength = 0.35f;
        bool showOctree = false;
        float heatViewMin = MIN_TEMP;
        float heatViewMax = MAX_TEMP;

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