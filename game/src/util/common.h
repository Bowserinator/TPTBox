#pragma once

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
