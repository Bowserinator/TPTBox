#ifndef INTERFACE_ICON_MANAGER_H
#define INTERFACE_ICON_MANAGER_H

#include "rlgl.h"
#include "raylib.h"
#include "icons.h"

class IconManager {
public:
    IconManager(IconManager& other) = delete;
    void operator=(const IconManager&) = delete;
    ~IconManager() { UnloadRenderTexture(texture); }

    void init() {
        texture = LoadRenderTexture(RAYGUI_ICON_MAX_ICONS * RAYGUI_ICON_SIZE, RAYGUI_ICON_SIZE);

        Image img = LoadImageFromMemory(".png", icons_png_data, icons_png_size);
        Texture imTex = LoadTextureFromImage(img);
        UnloadImage(img);
        SetTextureFilter(texture.texture, RL_TEXTURE_FILTER_BILINEAR);

        BeginTextureMode(texture);
        ClearBackground(BLANK);
        DrawTexture(imTex, 0, 0, WHITE);
        EndTextureMode();
        UnloadTexture(imTex);
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