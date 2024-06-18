#ifndef INTERFACE_BRUSH_PREVIEW_H_
#define INTERFACE_BRUSH_PREVIEW_H_

#include "raylib.h"
#include <array>

class Renderer;

namespace brush_preview {
    enum class BrushFace { X = 0, Y, Z };

    inline constexpr Vector3 createModeColor{ 107, 255, 112 };
    inline constexpr Vector3 deleteModeColor{ 255, 43, 43 };

    inline Shader model_shader = {0};
    inline int model_shader_depth_tex_loc = 0;
    inline int model_shader_res_loc = 0;
    inline RenderTexture2D brush_tool_render_tex = {0};

    /** Init shaders */
    void init();

    /**
     * @brief Setup the shader for the current frame
     * @param centerOfModel 
     * @param renderer
     */
    void setup_shader(const Vector3 centerOfModel, Renderer * renderer);

    /**
     * @brief Return 3 separate models for x,y, and z faces
     * @param width 
     * @param height 
     * @param length 
     * @return std::array<Model, 3> x,y,z faces
     */
    std::array<Model, 3> generate_cuboid_faces(float width, float height, float length);

    /**
     * @brief 
     * @param models X,Y,Z face models
     * @param center_of_model Center pos of model
     * @param delete_mode In delete mode?
     */
    void draw_mesh_faces(const std::array<Model, 3> &models, const Vector3 center_of_model, const bool delete_mode);

    /**
     * @brief Same as draw_mesh_faces but renders to render_tex
     * @param renderer 
     * @param render_tex
     * @param models 
     * @param center_of_model 
     * @param delete_mode 
     */
    void draw_mesh_faces_on_render_tex(Renderer * renderer, RenderTexture2D &render_tex,
        const std::array<Model, 3> &models, const Vector3 center_of_model, const bool delete_mode);
} // namespace brush_preview

#endif // INTERFACE_BRUSH_PREVIEW_H_
