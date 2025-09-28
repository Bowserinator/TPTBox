#pragma once

#include "rlgl.h"
#include <glad.h>

namespace util {
class GlTimeQuery {
public:
    GlTimeQuery() {
        glGenQueries(1, &m_query);
        glBeginQuery(GL_TIME_ELAPSED, m_query);
    }

    double time_elapsed_ms() {
        glEndQuery(GL_TIME_ELAPSED);
        int out;
        glGetQueryObjectiv(m_query, GL_QUERY_RESULT, &out);
        return out / static_cast<double>(1e6);
    }

private:
    unsigned int m_query = 0;
};
} // namespace util
