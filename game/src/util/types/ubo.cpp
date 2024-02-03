#include "ubo.h"

UBOWriter::UBOWriter(const GLuint program, const GLuint UBOId, const char * uniformBlockName):
        uniformBlockName(uniformBlockName),
        programId(program),
        UBOId(UBOId),
        data(nullptr) {
    GLuint uniformBlockIndex = glGetUniformBlockIndex(program, uniformBlockName);
    glGetActiveUniformBlockiv(program, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSizeBytes);
    glGetActiveUniformBlockiv(program, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);
    data = new uint8_t[dataSizeBytes];
}

GLuint UBOWriter::get_offset(const char * memberName) {
    const auto ix = glGetProgramResourceIndex(programId, GL_UNIFORM, memberName);
    const GLenum prop = GL_OFFSET;
    GLint offset = 0;
    glGetProgramResourceiv(programId, GL_UNIFORM, ix, 1, &prop, 1, NULL, &offset);
    return offset;
}
