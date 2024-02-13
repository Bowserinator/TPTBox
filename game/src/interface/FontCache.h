#ifndef FONT_CACHE_H
#define FONT_CACHE_H

#include "raylib.h"

constexpr float FONT_SIZE = 11.0f;
constexpr float FONT_SPACING = -0.5f;

class FontCache {
public:
    Font main_font;

    FontCache(FontCache &other) = delete;
    void operator=(const FontCache&) = delete;
    ~FontCache();

    // Call after OpenGL context has been initialized
    void init();

    static FontCache * ref() {
        if (single == nullptr) [[unlikely]]
            single = new FontCache;
        return single;
    };

    static void destroy() {
        delete single;
        single = nullptr;
    }
private:
    bool _init;
    static FontCache * single;

    FontCache(): _init(false) {}
};

#endif