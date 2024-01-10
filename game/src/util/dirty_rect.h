#ifndef UTIL_DIRTY_RECT
#define UTIL_DIRTY_RECT

#include "stdint.h"

namespace util {
    template <class T>
    class DirtyRect {
    public:
        T minX, maxX, minY, maxY;
        DirtyRect(): minX(std::numeric_limits<T>::max()), maxX(0), minY(std::numeric_limits<T>::max()), maxY(0) {}

        void reset() {
            minX = minY = std::numeric_limits<T>::max();
            maxX = maxY = 0;
        }

        void update(const T x, const T y) {
            minX = std::min(x, minX);
            minY = std::min(y, minY);
            maxX = std::max(x, maxX);
            maxY = std::max(y, maxY);
        }
    };
}

#endif