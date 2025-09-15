#pragma once

#include "raylib.h"
#include <string>

namespace util {
/// @brief Load text data from file, returns a '\0' terminated string using raylib's LoadTextFile
/// @param path Path to file to read
/// @return Contents of file as null terminated string
[[nodiscard]] inline std::string load_text_file(const std::string &path) {
    char *code = LoadFileText(path.c_str());
    std::string out;
    out.assign(code);
    return out;
}
} // namespace util
