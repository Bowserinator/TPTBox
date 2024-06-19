#include "str_format.h"
#include "../simulation/ElementClasses.h"

#include <algorithm>

namespace util {
    std::string unicode_to_utf8(unsigned int codepoint) {
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

    std::optional<float> parse_string_float(const std::string &s) {
        double val = 0.0f;
        if (!s.length()) return std::nullopt;

        auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
        bool isValidFloat = ec == std::errc() && p == s.data() + s.size();

        if (!isValidFloat) return std::nullopt;
        return val;
    }

    std::optional<float> temp_string_to_kelvin(const std::string &s) {
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

    std::optional<unsigned int> parse_string_part_type(const std::string &s) {
        auto tmp = util::parse_string_integer<unsigned int>(s);
        if (tmp != std::nullopt) {
            auto val = tmp.value();
            if (val >= ELEMENT_COUNT) return std::nullopt;
            return val;
        }

        auto str = s;
        std::transform(str.begin(), str.end(), str.begin(), toupper);

        for (unsigned int i = 0; i < GetElements().size(); i++) {
            auto &el = GetElements()[i];
            if (el.Name == str || el.Identifier == str)
                return i;
        }
        return std::nullopt;
    }

} // namespace util
