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

    extern std::string unicode_to_utf8(unsigned int codepoint);
    extern std::optional<float> parse_string_float(const std::string &s);
    extern std::optional<float> temp_string_to_kelvin(const std::string &s);
    extern std::optional<unsigned int> parse_string_part_type(const std::string &s);

    template <class T> requires std::is_integral_v<T>
    std::optional<T> parse_string_integer(const std::string &s) {
        if (!s.length()) return std::nullopt;
        if constexpr(std::unsigned_integral<T>) { // Can't have negative unsigned ints
            if (s[0] == '-') return std::nullopt;
        }
        int start = 0;
        bool is_hex = false;

        if (s[0] == '-')
            start = 1;
        if (s.length() > 2 + start && s[start] == '0' && tolower(s[start + 1]) == 'x') {
            start += 2;
            is_hex = true;
        }

        for (auto i = start; i < s.length(); i++)
            if (is_hex ? !isxdigit(s[i]) : !isdigit(s[i])) return std::nullopt;

        if (is_hex)
            return static_cast<T>(std::strtoull(s.c_str(), nullptr, 16));
        else {
            std::istringstream ss(s);
            T num;
            ss >> num;
            return num;
        }
    }
}

#endif // UTIL_STR_FORMAT_H_
