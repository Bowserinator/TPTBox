#include "multitexture.h"

#include <stdexcept>
#include <iostream>
#include <utility>

MultiTexture::MultiTexture(const unsigned int screenWidth, const unsigned int screenHeight):
        width(screenWidth), height(screenHeight), frameBuffer(0), colorTexture(0),
        glowOnlyTexture(0), blurOnlyTexture(0), depthTexture(0) {
    frameBuffer = rlLoadFramebuffer();

#ifdef DEBUG
    if (!frameBuffer)
        throw std::runtime_error("Failed to create framebuffer");
#endif

    rlEnableFramebuffer(frameBuffer);

    // Color renders, RGBA
    const auto format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    colorTexture    = rlLoadTexture(NULL, screenWidth, screenHeight, format, 1);
    glowOnlyTexture = rlLoadTexture(NULL, screenWidth, screenHeight, format, 1);
    blurOnlyTexture = rlLoadTexture(NULL, screenWidth, screenHeight, format, 1);

    rlTextureParameters(colorTexture, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(colorTexture, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(glowOnlyTexture, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(glowOnlyTexture, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(blurOnlyTexture, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(blurOnlyTexture, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_MIRROR_REPEAT);

    rlActiveDrawBuffers(3);
    rlFramebufferAttach(frameBuffer, colorTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(frameBuffer, glowOnlyTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(frameBuffer, blurOnlyTexture, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);

    depthTexture = rlLoadTextureDepth(screenWidth, screenHeight, false);
    rlFramebufferAttach(frameBuffer, depthTexture, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    // Make sure our framebuffer is complete.
    // NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
    // to rlDisableFramebuffer() here.
#ifdef DEBUG
    if (!rlFramebufferComplete(frameBuffer))
        throw std::runtime_error("Framebuffer is not complete");
#endif
}

MultiTexture::~MultiTexture() {
    if (frameBuffer) {
        rlUnloadFramebuffer(frameBuffer);
        rlUnloadTexture(colorTexture);
        rlUnloadTexture(glowOnlyTexture);
        rlUnloadTexture(blurOnlyTexture);
        rlUnloadTexture(depthTexture);
    }
}

MultiTexture::MultiTexture(MultiTexture &&other) {
    this->swap(other);
}

MultiTexture& MultiTexture::operator=(MultiTexture &&other) {
    if (&other != this)
        this->swap(other);
    return *this;
}

void MultiTexture::swap(MultiTexture &other) {
    std::swap(other.frameBuffer, frameBuffer);
    std::swap(other.colorTexture, colorTexture);
    std::swap(other.glowOnlyTexture, glowOnlyTexture);
    std::swap(other.blurOnlyTexture, blurOnlyTexture);
    std::swap(other.depthTexture, depthTexture);
    std::swap(other.width, width);
    std::swap(other.height, height);
}
