#ifndef UTIL_STR_FORMAT_H
#define UTIL_STR_FORMAT_H

#include <bitset>
#include <string>
#include <sstream>
#include <locale>

namespace util {
    template <int N>
    std::string bitset_to_str(const std::bitset<N> bits) {
        std::string out(N, ' ');
        for (int i = 0; i < N; i++)
            out[i] = bits[i] ? '1' : '0';
        return out;
    }

    template <class T> requires std::integral<T> or std::floating_point<T>
    std::string format_commas(T num) {
        std::string out = std::to_string(num);
        for (int i = out.length() - 3; i > 0; i -= 3)
            out.insert(i, ",");
        return out;
    }
}

#endif