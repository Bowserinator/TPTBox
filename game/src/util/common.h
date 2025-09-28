#pragma once

// Always include rlgl.h before raylib.h
// clang-format off
#include "rlgl.h"
#include "raylib.h"
// clang-format on
#include <exception>
#include <format>
#include <string>
#include <variant>

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

// Type | match { [](T& a) {}, ... }
template <class... Ts> using match = overloaded<Ts...>;
template <typename... Ts, typename... Fs>
constexpr decltype(auto) operator|(std::variant<Ts...> &v, match<Fs...> const &match) {
    return std::visit(match, v);
}
template <typename... Ts, typename... Fs>
constexpr decltype(auto) operator|(const std::variant<Ts...> &v, match<Fs...> const &match) {
    return std::visit(match, v);
}

/// Type | is<T>
template <class T> struct is_operator {};
template <class T> constexpr is_operator<T> is{};
template <class... Ts, class T> constexpr bool operator|(std::variant<Ts...> const &v, is_operator<T>) {
    return std::holds_alternative<T>(v);
}

template <class T, class U> constexpr bool operator|(const T *src, is_operator<U>) {
    if constexpr (std::is_base_of_v<U, std::remove_pointer_t<T>> && std::is_polymorphic_v<std::remove_pointer_t<T>>)
        return dynamic_cast<U *>(src) != nullptr;
    else if constexpr (std::is_convertible_v<T *, U *>)
        return true;
    return false;
}

/// Type | as<T>
template <class T> struct as_operator {};
template <class T> constexpr as_operator<T> as{};
template <class... Ts, class T> constexpr decltype(auto) operator|(std::variant<Ts...> const &v, as_operator<T>) {
    return std::get<T>(v);
}
template <class T, class U> constexpr decltype(auto) operator|(const T &v, as_operator<U>) { return static_cast<U>(v); }

#define DEBUG_MSG(msg) std::format("[{}:{}] {}", __FILE__, __LINE__, msg)

namespace util {
/// 3d array begin() and end()
template <typename T, size_t N1, size_t N2, size_t N3> T *t3d_begin(T (&arr)[N1][N2][N3]) {
    return reinterpret_cast<T *>(arr);
}
template <typename T, size_t N1, size_t N2, size_t N3> T *t3d_end(T (&arr)[N1][N2][N3]) {
    return reinterpret_cast<T *>(arr) + N1 * N2 * N3;
}

/// Terminate program with fatal error
inline void die(const std::string &reason) {
#ifdef DEBUG
    throw std::runtime_error(reason);
#else
    TraceLog(LOG_ERROR, reason.c_str());
    std::terminate();
#endif
}
} // namespace util