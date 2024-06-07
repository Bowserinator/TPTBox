#include "graphics.h"

void util::upload_mesh_vertices_only(Mesh *mesh, bool dynamic) {
    if (mesh->vaoId > 0) { // Check if mesh has already been loaded in GPU
        TRACELOG(LOG_WARNING, "VAO: [ID %i] Trying to re-load an already loaded mesh", mesh->vaoId);
        return;
    }

    mesh->vaoId = 0;        // Vertex Array Object
    mesh->vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));
    mesh->vboId[0] = 0;     // Vertex buffer: positions
    mesh->vboId[1] = 0;     // Vertex buffer: texcoords
    mesh->vboId[2] = 0;     // Vertex buffer: normals
    mesh->vboId[3] = 0;     // Vertex buffer: colors
    mesh->vboId[4] = 0;     // Vertex buffer: tangents
    mesh->vboId[5] = 0;     // Vertex buffer: texcoords2
    mesh->vboId[6] = 0;     // Vertex buffer: indices

#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
    mesh->vaoId = rlLoadVertexArray();
    rlEnableVertexArray(mesh->vaoId);

    // NOTE: Vertex attributes must be uploaded considering default locations points and available vertex data
    // Enable vertex attributes: position (shader-location = 0)
    void *vertices = (mesh->animVertices != NULL)? mesh->animVertices : mesh->vertices;
    mesh->vboId[0] = rlLoadVertexBuffer(vertices, mesh->vertexCount*3*sizeof(float), dynamic);
    rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 0, 0);
    rlEnableVertexAttribute(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    {
        float value[2] = { 1.0f, 1.0f };
        rlSetVertexAttributeDefault(1, value, SHADER_ATTRIB_VEC2, 2);
        rlDisableVertexAttribute(1);
    }

    // Default vertex attribute: normal
    {
        float value[3] = { 1.0f, 1.0f, 1.0f };
        rlSetVertexAttributeDefault(2, value, SHADER_ATTRIB_VEC3, 3);
        rlDisableVertexAttribute(2);
    }

    // Default vertex attribute: color
    {
        float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };    // WHITE
        rlSetVertexAttributeDefault(3, value, SHADER_ATTRIB_VEC4, 4);
        rlDisableVertexAttribute(3);
    }


    // Default vertex attribute: tangent
    // WARNING: Default value provided to shader if location available
    {
        float value[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        rlSetVertexAttributeDefault(4, value, SHADER_ATTRIB_VEC4, 4);
        rlDisableVertexAttribute(4);
    }

    // Default vertex attribute: texcoord2
    // WARNING: Default value provided to shader if location available
    {
        float value[2] = { 0.0f, 0.0f };
        rlSetVertexAttributeDefault(5, value, SHADER_ATTRIB_VEC2, 2);
        rlDisableVertexAttribute(5);
    }

    if (mesh->vaoId > 0) TRACELOG(LOG_INFO, "VAO: [ID %i] Mesh uploaded successfully to VRAM (GPU)", mesh->vaoId);
    else TRACELOG(LOG_INFO, "VBO: Mesh uploaded successfully to VRAM (GPU)");

    rlDisableVertexArray();
#endif
}
