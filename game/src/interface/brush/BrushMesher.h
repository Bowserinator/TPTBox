#ifndef BRUSH_MESHER_H
#define BRUSH_MESHER_H

#include "raylib.h"
#include "../../util/vector_op.h"
#include "Brushes.h"

#include <array>

enum class BrushFace { X = 0, Y, Z };
struct BrushFaceModels {
    std::array<Model, 3> models;
    Vector3T<int> size{ 0, 0, 0 };

    inline static Shader BRUSH_MODEL_SHADER = {0};
    inline static int BRUSH_MODEL_SHADER_MV_LOC = 0;

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
     * @param deleteMode Whether we are currently in delete mode (changes graphics)
     */
    void draw(Vector3T<int> center, bool deleteMode);

    /**
     * @brief Unload models in struct
     */
    void unload();
};

#endif