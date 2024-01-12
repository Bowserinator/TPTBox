#include "FontCache.h"
#include "raylib.h"

#include <iostream>

FontCache * FontCache::single = nullptr;

void FontCache::init() {
    if (_init) return;

    main_font = LoadFontEx("resources/dogicapixel.ttf", FONT_SIZE, 0, 250);

    _init = true;
}

FontCache::~FontCache() {
    UnloadFont(main_font);
    std::cout << "destroyed\n";
}
