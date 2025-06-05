#include "depthtexture.h"

#include <stdexcept>

DepthTexture::DepthTexture(const unsigned int screenWidth, const unsigned int screenHeight):
        width(screenWidth), height(screenHeight), colorTexture(0), depthTexture(0) {
    target.id = rlLoadFramebuffer();

#ifdef DEBUG
    if (!target.id)
        throw std::runtime_error("Failed to create framebuffer");
#endif

    rlEnableFramebuffer(target.id);

    // Color renders, RGBA
    const auto format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    colorTexture = rlLoadTexture(NULL, screenWidth, screenHeight, format, 1);

    target.texture.id = colorTexture;
    target.texture.width = width;
    target.texture.height = height;
    target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.texture.mipmaps = 1;

    rlTextureParameters(colorTexture, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_MIRROR_REPEAT);
    rlTextureParameters(colorTexture, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_MIRROR_REPEAT);

    rlActiveDrawBuffers(1);
    rlFramebufferAttach(target.id, colorTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

    depthTexture = rlLoadTextureDepth(screenWidth, screenHeight, false);
    target.depth.id = depthTexture;
    target.depth.width = width;
    target.depth.height = height;
    target.depth.format = 19; // DEPTH_COMPONENT_24BIT
    target.depth.mipmaps = 1;
    rlFramebufferAttach(target.id, depthTexture, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    // Make sure our framebuffer is complete.
    // NOTE: rlFramebufferComplete() automatically unbinds the framebuffer, so we don't have
    // to rlDisableFramebuffer() here.
#ifdef DEBUG
    if (!rlFramebufferComplete(target.id))
        throw std::runtime_error("Framebuffer is not complete");
#endif
}

DepthTexture::~DepthTexture() {
    if (target.id) {
        rlUnloadFramebuffer(target.id);
        rlUnloadTexture(colorTexture);
        rlUnloadTexture(depthTexture);
    }
}

DepthTexture::DepthTexture(DepthTexture &&other) {
    this->swap(other);
}

DepthTexture& DepthTexture::operator=(DepthTexture &&other) {
    if (&other != this)
        this->swap(other);
    return *this;
}

void DepthTexture::swap(DepthTexture &other) {
    std::swap(other.target, target);
    std::swap(other.colorTexture, colorTexture);
    std::swap(other.depthTexture, depthTexture);
    std::swap(other.width, width);
    std::swap(other.height, height);
}
