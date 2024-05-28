#ifndef UTIL_STR_FORMAT_H
#define UTIL_STR_FORMAT_H

#include "./types/bitset8.h"
#include <string>
#include <sstream>
#include <locale>

namespace util {
    template <int N>
    std::string bitset_to_str(const util::Bitset8 bits) {
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

    inline std::string unicode_to_utf8(unsigned int codepoint) {
        std::string out;

        if (codepoint <= 0x7f)
            out.append(1, static_cast<char>(codepoint));
        else if (codepoint <= 0x7ff) {
            out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else if (codepoint <= 0xffff) {
            out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else {
            out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return out;
    }
}

#endif