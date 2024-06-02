#include "BrushMesher.h"
#include "../../render/constants.h"
#include "../../util/graphics.h"
#include "../../simulation/SimulationDef.h"
#include <vector>

BrushFaceModels BrushFaceModels::GenBrushModel(const Brush &brush, Vector3T<int> size) {
    if (!IsShaderReady(BRUSH_MODEL_SHADER)) {
        #ifdef EMBED_SHADERS
        #include "../../resources/shaders/generated/brush_clip.vs.h"
        BRUSH_MODEL_SHADER = LoadShaderFromMemory(brush_clip_vs_source, nullptr);
        #else
        BRUSH_MODEL_SHADER = LoadShader("resources/shaders/brush_clip.vs", nullptr);
        #endif

        BRUSH_MODEL_SHADER_MV_LOC = GetShaderLocation(BRUSH_MODEL_SHADER, "mv");
        util::set_shader_value(BRUSH_MODEL_SHADER, GetShaderLocation(BRUSH_MODEL_SHADER, "simRes"),
            Vector3{ (float)XRES, (float)YRES, (float)ZRES });
    }

    if (size.x % 2 == 0) size.x++;
    if (size.y % 2 == 0) size.y++;
    if (size.z % 2 == 0) size.z++;

    std::vector<unsigned char> voxel_fill_map(size.x * size.y * size.z, 0);
    std::vector<unsigned char> visited_map((size.x + 2) * (size.y + 2) * (size.z + 2), 0);

    auto idx = [size](int x, int y, int z) { return x + y * size.x + z * size.x * size.y; };
    auto idx2 = [size](int x, int y, int z) { return (x + 1) + (y + 1) * (size.x + 2) + (z + 1) * (size.x + 2) * (size.y + 2); };
    auto is_filled = [size, idx, &voxel_fill_map](int x, int y, int z) {
        if (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z)
            return false;
        return voxel_fill_map[idx(x, y, z)] != 0;
    };

    #pragma omp parallel for
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        Vector3T<int> subpos{ x - size.x / 2, y - size.y / 2, z - size.z / 2 };

        if (brush.map((Vector3)subpos, (Vector3)size, Vector3{0.0f, 0.0f, 0.0f}))
            voxel_fill_map[idx(x, y, z)] = 1;
    }

    BrushFaceModels out{
        .models = {},
        .size = size
    };
    std::array<std::vector<float>, 3> vertices;

    auto add_vertex = [&](BrushFace face, int x, int y, int z) {
        vertices[(int)face].push_back(x);
        vertices[(int)face].push_back(y);
        vertices[(int)face].push_back(z);
    };

    for (int z = 0; z <= size.z; z++) {
    for (int y = 0; y <= size.y; y++) {
    for (int x = 0; x <= size.x; x++) {
        // Top faces
        int x2 = x;
        if (!is_filled(x2, y, z) && is_filled(x2, y - 1, z)) {
            if ((visited_map[idx2(x2, y, z)] & (1 << 0)) != 0)
                break;
            visited_map[idx2(x2, y, z)] |= (1 << 0);
            x2++;
        }
        if (x2 > x) {
            add_vertex(BrushFace::Y, x, y, z);
            add_vertex(BrushFace::Y, x, y, z + 1);
            add_vertex(BrushFace::Y, x2, y, z);
            
            add_vertex(BrushFace::Y, x2, y, z);
            add_vertex(BrushFace::Y, x, y, z + 1);
            add_vertex(BrushFace::Y, x2, y, z + 1);
        }

        // Bottom faces
        x2 = x;
        if (is_filled(x2, y, z) && !is_filled(x2, y - 1, z)) {
            if ((visited_map[idx2(x2, y, z)] & (1 << 1)) != 0)
                break;
            visited_map[idx2(x2, y, z)] |= (1 << 1);
            x2++;
        }
        if (x2 > x) {
            add_vertex(BrushFace::Y, x, y, z);
            add_vertex(BrushFace::Y, x2, y, z);
            add_vertex(BrushFace::Y, x, y, z + 1);

            add_vertex(BrushFace::Y, x2, y, z);
            add_vertex(BrushFace::Y, x2, y, z + 1);
            add_vertex(BrushFace::Y, x, y, z + 1);
        }

        // Left faces
        int y2 = y;
        if (!is_filled(x, y2, z) && is_filled(x - 1, y2, z)) {
            if ((visited_map[idx2(x, y2, z)] & (1 << 2)) != 0)
                break;
            visited_map[idx2(x, y2, z)] |= (1 << 2);
            y2++;
        }
        if (y2 > y) {
            add_vertex(BrushFace::X, x, y, z);
            add_vertex(BrushFace::X, x, y2, z);
            add_vertex(BrushFace::X, x, y, z + 1);

            add_vertex(BrushFace::X, x, y2, z);
            add_vertex(BrushFace::X, x, y2, z + 1);
            add_vertex(BrushFace::X, x, y, z + 1);
        }

        // Right faces
        y2 = y;
        if (is_filled(x, y2, z) && !is_filled(x - 1, y2, z)) {
            if ((visited_map[idx2(x, y2, z)] & (1 << 3)) != 0)
                break;
            visited_map[idx2(x, y2, z)] |= (1 << 3);
            y2++;
        }
        if (y2 > y) {
            add_vertex(BrushFace::X, x, y, z);
            add_vertex(BrushFace::X, x, y, z + 1);
            add_vertex(BrushFace::X, x, y2, z);
            
            add_vertex(BrushFace::X, x, y2, z);
            add_vertex(BrushFace::X, x, y, z + 1);
            add_vertex(BrushFace::X, x, y2, z + 1);
        }

        // Front faces
        x2 = x;
        if (!is_filled(x2, y, z) && is_filled(x2, y, z - 1)) {
            if ((visited_map[idx2(x2, y, z)] & (1 << 4)) != 0)
                break;
            visited_map[idx2(x2, y, z)] |= (1 << 4);
            x2++;
        }
        if (x2 > x) {
            add_vertex(BrushFace::Z, x, y, z);
            add_vertex(BrushFace::Z, x2, y, z);
            add_vertex(BrushFace::Z, x, y + 1, z);

            add_vertex(BrushFace::Z, x2, y, z);
            add_vertex(BrushFace::Z, x2, y + 1, z);
            add_vertex(BrushFace::Z, x, y + 1, z);
        }

        // Back faces
        x2 = x;
        if (is_filled(x2, y, z) && !is_filled(x2, y, z - 1)) {
            if ((visited_map[idx2(x2, y, z)] & (1 << 5)) != 0)
                break;
            visited_map[idx2(x2, y, z)] |= (1 << 5);
            x2++;
        }
        if (x2 > x) {
            add_vertex(BrushFace::Z, x, y, z);
            add_vertex(BrushFace::Z, x, y + 1, z);
            add_vertex(BrushFace::Z, x2, y, z);
            
            add_vertex(BrushFace::Z, x2, y, z);
            add_vertex(BrushFace::Z, x, y + 1, z);
            add_vertex(BrushFace::Z, x2, y + 1, z);
        }
    }}}

    for (int i = 0; i < 3; i++) {
        Mesh mesh = { 0 };

        mesh.triangleCount = vertices[i].size() / 9;
        mesh.vertexCount = mesh.triangleCount * 3;
        mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
        mesh.texcoords = nullptr;
        mesh.normals = nullptr;
        memcpy(mesh.vertices, &vertices[i][0], mesh.vertexCount * 3 * sizeof(float));

        UploadMesh(&mesh, false);
        out.models[i] = LoadModelFromMesh(mesh);
        out.models[i].materials[0].shader = BRUSH_MODEL_SHADER;
    }

    return out;
}

void BrushFaceModels::draw(Vector3T<int> center, bool deleteMode) {
    constexpr Vector3 createModeColor{ 107, 255, 112 };
    constexpr Vector3 deleteModeColor{ 255, 43, 43 };
    const Vector3 centerOfModel = Vector3{
        static_cast<float>(center.x - (int)size.x / 2),
        static_cast<float>(center.y - (int)size.y / 2),
        static_cast<float>(center.z - (int)size.z / 2)
    };

    SetShaderValueMatrix(BRUSH_MODEL_SHADER, BRUSH_MODEL_SHADER_MV_LOC,
        MatrixTranslate(
            centerOfModel.x,
            centerOfModel.y,
            centerOfModel.z
        ));

    for (int i = 0; i < models.size(); i++) {
        const unsigned char faceShadow = (i == 0) ? 255 : (i == 1) ? 220 : 150;
        const Vector3 col = deleteMode ? deleteModeColor : createModeColor;

        DrawModelWires(models[i],
            centerOfModel, 0.99f,
            Color{
                static_cast<unsigned char>(col.x / 255.0f * faceShadow),
                static_cast<unsigned char>(col.y / 255.0f * faceShadow),
                static_cast<unsigned char>(col.z / 255.0f * faceShadow),
                120
            });
    }

    DrawCubeWires((Vector3)center, size.x, size.y, size.z, GRAY);
}

void BrushFaceModels::unload() {
    for (int i = 0; i < models.size(); i++)
        UnloadModel(models[i]);
}
