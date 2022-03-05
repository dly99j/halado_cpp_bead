//
// Created by dly99j on 2/12/22.
//

#ifndef HALADO_CPP_BEAD_UTILITY_HPP
#define HALADO_CPP_BEAD_UTILITY_HPP

#include <type_traits>
#include <string_view>
#include <stdexcept>

/*
template<typename T>
concept integral = std::is_integral_v<T>;

template<integral T>
auto func() { ... }
*/

template<class T>
constexpr auto check_integral_type(std::string_view error_msg) {
    if constexpr (!std::is_integral_v< T >) {
        throw std::invalid_argument(error_msg.data());
    }
}

template<class T, class U>
constexpr auto check_different_type(std::string_view error_msg) {
    if constexpr (std::is_same_v< T, U >) {
        throw std::invalid_argument(error_msg.data());
    }
}

template<class Mapped, class Key>
constexpr auto map_constructor_check() {
    check_integral_type<Key>("Key is not integral type");
    check_different_type<Mapped, Key>("Mapped type and Key type are the same");
}

#endif //HALADO_CPP_BEAD_UTILITY_HPP
