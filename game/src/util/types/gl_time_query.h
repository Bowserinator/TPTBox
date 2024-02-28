#ifndef UTIL_GL_TIME_QUERY_H
#define UTIL_GL_TIME_QUERY_H

#include "rlgl.h"
#include <glad.h>

namespace util {
    class GlTimeQuery {
    public:
        GlTimeQuery() {
            glGenQueries(1, &query);
            glBeginQuery(GL_TIME_ELAPSED, query);
        }

        double timeElapsedMs() {
            glEndQuery(GL_TIME_ELAPSED);
            int out;
            glGetQueryObjectiv(query, GL_QUERY_RESULT, &out);
            return out / static_cast<double>(1e6);
        }
    private:
        unsigned int query = 0;
    };
}

#endif