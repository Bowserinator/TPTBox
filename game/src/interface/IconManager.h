#ifndef INTERFACE_ICON_MANAGER_H
#define INTERFACE_ICON_MANAGER_H

#include "rlgl.h"
#include "raylib.h"
#include "iconset.rgi.h"

class IconManager {
public:
    IconManager(IconManager& other) = delete;
    void operator=(const IconManager&) = delete;
    ~IconManager() { UnloadRenderTexture(texture); }

    void init() {
        texture = LoadRenderTexture(RAYGUI_ICON_MAX_ICONS * RAYGUI_ICON_SIZE, RAYGUI_ICON_SIZE);
        SetTextureFilter(texture.texture, RL_TEXTURE_FILTER_LINEAR);

        BeginTextureMode(texture);
        ClearBackground(Color{0, 0, 0, 0});
        for (auto icon = 0; icon < RAYGUI_ICON_MAX_ICONS; icon++) {
            for (int bit = 0; bit < RAYGUI_ICON_SIZE * RAYGUI_ICON_SIZE; bit++) {
                int i = bit / 32;
                int j = bit % 32;
                if (guiIcons[RAYGUI_ICON_DATA_ELEMENTS * icon + i] & (1 << j))
                    DrawPixel(RAYGUI_ICON_SIZE * icon + bit % RAYGUI_ICON_SIZE, bit / RAYGUI_ICON_SIZE, WHITE);
            }
        }
        EndTextureMode();
    }

    void draw(float x, float y, guiIconName icon, Color color = WHITE, float size = RAYGUI_ICON_SIZE) {
        DrawTexturePro(texture.texture,
            Rectangle { (float)icon * RAYGUI_ICON_SIZE, 0, RAYGUI_ICON_SIZE, -RAYGUI_ICON_SIZE },
            Rectangle { x, y, size, size },
            Vector2{0, 0}, 0.0f, color);
    }

    static IconManager * ref() {
        if (single == nullptr) [[unlikely]]
            single = new IconManager;
        return single;
    };
private:
    inline static IconManager * single = nullptr;
    RenderTexture2D texture = {0};

    IconManager() {}
};

#endif