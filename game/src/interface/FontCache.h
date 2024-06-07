#ifndef INTERFACE_FONTCACHE_H_
#define INTERFACE_FONTCACHE_H_

#include "raylib.h"

constexpr float FONT_SIZE = 9.0f;
constexpr float FONT_SPACING = -0.5f;

class FontCache {
public:
    Font main_font;

    FontCache(const FontCache &other) = delete;
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

#endif // INTERFACE_FONTCACHE_H_
