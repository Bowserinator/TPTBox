#include "ubo.h"

UBOWriter::UBOWriter(const GLuint program, const GLuint UBOId, const char * uniform_block_name):
        m_uniform_block_name(uniform_block_name),
        m_program_id(program),
        m_ubo_id(UBOId),
        m_data(nullptr) {
    GLuint uniform_block_index = glGetUniformBlockIndex(program, uniform_block_name);
    glGetActiveUniformBlockiv(program, uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &m_data_size_bytes);
    glGetActiveUniformBlockiv(program, uniform_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &m_uniform_count);
    m_data = new uint8_t[m_data_size_bytes];
}

GLuint UBOWriter::get_offset(const char * member_name) {
    const auto ix = glGetProgramResourceIndex(m_program_id, GL_UNIFORM, member_name);
    const GLenum prop = GL_OFFSET;
    GLint offset = 0;
    glGetProgramResourceiv(m_program_id, GL_UNIFORM, ix, 1, &prop, 1, NULL, &offset);
    return offset;
}
