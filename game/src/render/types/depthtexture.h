#pragma once

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>
#include <utility>

class DepthTexture {
public:
    DepthTexture():
        width(0), height(0), depthTexture(0) {}
    DepthTexture(const unsigned int screenWidth, const unsigned int screenHeight);
    ~DepthTexture();

    DepthTexture(const DepthTexture &other) = delete;
    DepthTexture &operator=(const DepthTexture &other) = delete;
    DepthTexture(DepthTexture &&other);
    DepthTexture &operator=(DepthTexture &&other);

    void swap(DepthTexture &other);

    RenderTexture2D target = { 0 };
    unsigned int width, height;
    unsigned int colorTexture;
    unsigned int depthTexture;
};
