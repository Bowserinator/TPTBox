#pragma once

#include "raylib.h"
#include "rlgl.h"
#include "util/common.h"

#include <cstdint>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <map>

namespace util {

enum class ShaderType : int { FRAGMENT = RL_FRAGMENT_SHADER, VERTEX = RL_VERTEX_SHADER, COMPUTE = RL_COMPUTE_SHADER };

/// @brief Stores source code for shader, can be used to
///        dynamically generate shader code
class TPBShaderSourceCode {
public:
    enum Restriction { NONE, READONLY, WRITEONLY };
    using glsl_base_t = std::variant<bool, int32_t, uint32_t, float, double>;

    TPBShaderSourceCode(const std::string &src);

    /// @brief Update the internal code
    TPBShaderSourceCode &update();

    /// @brief Add a non-vector constant to the shader definition with name
    ///        Note: does not prevent duplicate constant names
    TPBShaderSourceCode &add_const(const std::string &name, const glsl_base_t &value);

    /// @brief Add a constant vector of a glsl base type to the shader definition
    ///        Note: does not prevent duplicate constant names
    template <class T, size_t N>
    TPBShaderSourceCode &add_const_arr(const std::string &name, const std::array<T, N> &value) {
        static_assert(N > 0);
        glsl_base_t dummy = value[0]; // Will fail if T is not in glsl_t variant
        const std::string vec_name =
            dummy | is<float> ? std::format("vec{}", N) : std::format("{}vec{}", glsl_t_to_name(dummy)[0], N);

        std::string contents = "";
        for (size_t i = 0; i < N; i++) {
            contents += std::to_string(value[i]);
            if (N > 1 && i < N - 1) contents += ",";
        }
        m_more_lines.push_back(std::format("const {} {} = {}({});", vec_name, name, vec_name, contents));
        return *this;
    }

    /// @brief Add a new variable binding, members is struct members directly interned in the code
    TPBShaderSourceCode &add_binding(unsigned int binding, const std::string &members, Restriction restrictions = NONE,
                                 const std::string_view layout = "std430");

    [[nodiscard]] operator std::string() const { return m_code; }
    [[nodiscard]] operator const char *() const { return m_code.c_str(); }

    /// @brief Get glsl type name for base type, ie `12.5f` -> `"float"`
    inline static const char *glsl_t_to_name(const glsl_base_t &val) {
        return val | match{
                         [](bool) { return "bool"; },     [](int32_t) { return "int"; },
                         [](uint32_t) { return "uint"; }, [](float) { return "float"; },
                         [](double) { return "double"; },
                     };
    }

private:
    std::string m_code;
    std::vector<std::string> m_more_lines;
};


/// @brief Uniform manager for a given shader
class UniformManager {
public:
    UniformManager() = default;
    UniformManager(Shader &shader): m_shader(std::ref(shader)) {}

    /// Get uniform by location, throws std::out_of_range if name is not a valid uniform
    /// for the given shader (DEBUG mode only)
    [[nodiscard]] int get(const std::string &name);
private:
    std::optional<std::reference_wrapper<Shader>> m_shader;
    std::map<std::string, int> m_uniform_locs;
};

/// @brief Single shader with given program code
class TPBComputeShader {
public:
    TPBComputeShader() = default;

    /// @brief Construct shader
    /// @param program Shader source code
    /// @param type Shader type
    TPBComputeShader(const std::string &program);

    [[nodiscard]] unsigned int id() const noexcept { return m_id; }

private:
    unsigned int m_id = INT_MAX;
};
} // namespace util
