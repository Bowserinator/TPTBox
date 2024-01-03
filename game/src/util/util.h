#ifndef UTIL_UTIL_H
#define UTIL_UTIL_H

namespace util {
    /**
     * @brief Round a float fast. WARNING: pass in only positive values
     * @param r A POSITIVE float
     * @return constexpr unsigned int 
     */
    constexpr unsigned int roundf(const float r) { return (unsigned int)(r + 0.5f); }
}

#endif