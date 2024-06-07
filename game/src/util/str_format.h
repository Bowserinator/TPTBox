#ifndef UTIL_STR_FORMAT_H_
#define UTIL_STR_FORMAT_H_

#include "./types/bitset8.h"
#include <string>
#include <sstream>
#include <locale>
#include <charconv>
#include <optional>

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

    inline std::optional<float> parse_string_float(const std::string &s) {
        double val = 0.0f;
        if (!s.length()) return std::nullopt;

        auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
        bool isValidFloat = ec == std::errc() && p == s.data() + s.size();

        if (!isValidFloat) return std::nullopt;
        return val;
    }

    inline std::optional<float> temp_string_to_kelvin(const std::string &s) {
        if (!s.length()) return std::nullopt;

        auto val = parse_string_float(s);
        if (val.has_value()) return val;
        val = parse_string_float(s.substr(0, s.length() - 1));
        if (!val.has_value()) return std::nullopt;

        const char last = s[s.length() - 1];
        if (last == 'C' || last == 'c')
            return val.value() + 273.15f;
        else if (last == 'F' || last == 'f')
            return (val.value() - 32.0f) * 5.0f / 9.0f + 273.15f;
        return val;
    }
}

#endif // UTIL_STR_FORMAT_H_
