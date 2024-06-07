#ifndef RENDER_TYPES_MULTITEXTURE_H_
#define RENDER_TYPES_MULTITEXTURE_H_

#include "raylib.h"
#include "rlgl.h"
#include <glad.h>

class MultiTexture {
public:
    MultiTexture():
        width(0), height(0), frameBuffer(0), colorTexture(0),
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

#endif // RENDER_TYPES_MULTITEXTURE_H_
