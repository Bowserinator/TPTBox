#ifndef UTIL_GRAPHICS_H
#define UTIL_GRAPHICS_H

#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"
#include "vector_op.h"

namespace util {
    // Quick assignment for shader vector values
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector2 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_VEC2);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector3 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_VEC3);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector3T<int> vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_IVEC3);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector4 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_IVEC4);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, float val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_FLOAT);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, int val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_INT);
    }

    // Draw render texture
    inline void draw_render_texture(const RenderTexture2D &tex, const Vector2 origin, const Vector2 size) {
        const Rectangle source{ 0.0f, 0.0f, (float)tex.texture.width, -(float)tex.texture.height };
        const Rectangle dest{ origin.x, origin.y, size.x, size.y };
        DrawTexturePro(tex.texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    inline void draw_render_texture(const RenderTexture2D &tex) {
        draw_render_texture(tex, Vector2{0.0f, 0.0f}, Vector2{ (float)tex.texture.width, (float)tex.texture.height });
    }

    // Custom load render textures
    inline RenderTexture2D load_render_texture_only_color(int width, int height, int format) {
        RenderTexture2D target = { 0 };
        target.id = rlLoadFramebuffer();

        if (target.id > 0) {
            rlEnableFramebuffer(target.id);

            target.texture.id = rlLoadTexture(NULL, width, height, format, 1);
            target.texture.width = width;
            target.texture.height = height;
            target.texture.format = format;
            target.texture.mipmaps = 1;

            rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
            if (rlFramebufferComplete(target.id))
                TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

            rlDisableFramebuffer();
        }
        else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");
        return target;
    }

    // Draw a triangle, vertices meant to be changed by vertex shader
    inline void draw_dummy_triangle() {
        rlBegin(RL_TRIANGLES);
            rlVertex3f(0, 0, 0);
            rlVertex3f(0, 0, 0);
            rlVertex3f(0, 0, 0);
        rlEnd();
    }
}

#endif