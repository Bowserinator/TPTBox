#ifndef RENDER_GBUFFER_H
#define RENDER_GBUFFER_H

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

class MultiTexture {
public:
    MultiTexture():
        frameBuffer(0), width(0), height(0), colorTexture(0),
        glowOnlyTexture(0), blurOnlyTexture(0), depthTexture(0) {}
    MultiTexture(const unsigned int screenWidth, const unsigned int screenHeight);
    ~MultiTexture();

    MultiTexture(const MultiTexture &other) = delete;
    MultiTexture &operator=(const MultiTexture &other) = delete;
    MultiTexture(MultiTexture &&other);
    MultiTexture &operator=(MultiTexture &&other);

    void swap(MultiTexture &other);

    unsigned int width, height;
    unsigned int frameBuffer;
    unsigned int colorTexture;
    unsigned int glowOnlyTexture;
    unsigned int blurOnlyTexture;
    unsigned int depthTexture;
};

#endif