#ifndef BRUSH_MESHER_H
#define BRUSH_MESHER_H

#include "raylib.h"
#include "../../util/vector_op.h"
#include "Brushes.h"

#include <array>

class Renderer;

enum class BrushFace { X = 0, Y, Z };
struct BrushFaceModels {
    std::array<Model, 3> models;
    Vector3T<int> size{ 0, 0, 0 };
    RenderTexture2D render_tex;

    inline static Shader brush_model_shader = {0};
    inline static int brush_model_shader_mv_loc = 0;
    inline static int brush_model_shader_depth_tex_loc = 0;
    inline static int brush_model_shader_res_loc = 0;

    /**
     * @brief Generate a brush model
     * @param brush Brush from Bushes.h
     * @param size Size of bounding box
     * @return BrushFaceModels 
     */
    static BrushFaceModels GenBrushModel(const Brush &brush, Vector3T<int> size);

    /**
     * @brief Draw the current model
     * @param center Center of the brush 
     * @param renderer
     * @param deleteMode Whether we are currently in delete mode (changes graphics)
     */
    void draw(Vector3T<int> center, Renderer * renderer, bool deleteMode);

    /**
     * @brief Unload models in struct
     */
    void unload();
};

#endif