#include "FontCache.h"
#include "raylib.h"
#include "./../resources/dogicapixel.h"

FontCache * FontCache::single = nullptr;

void FontCache::init() {
    if (_init) return;

    main_font = LoadFont_Dogicapixel();

    _init = true;
}

FontCache::~FontCache() {
    // This segfaults so screw it let the OS deal with it
    // UnloadFont(main_font);
}
