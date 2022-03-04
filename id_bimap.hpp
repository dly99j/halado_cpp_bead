//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include <type_traits>
#include "avl_tree.hpp"

//TODO check if integral type and if comparable, exception if not (this is ugly! - i mean the exception, the solution is nice)
//I might need my own exceptions
template<   class Mapped,
            class Key = std::size_t,
            class Compare = std::less<Key>,
            class Allocator = std::allocator<std::pair<const Key, Mapped> >,
            class = typename std::enable_if_t< std::is_integral_v<Key> >,
            class = typename std::enable_if_t< !std::is_same< Mapped, Key >::value > >
class id_bimap {
public:

    using mapped_type = Mapped;
    using key_type = Key;
    using value_type = std::pair<Key, Mapped>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t; //questionmark? TODO look this up
    using key_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    //TODO other member types

    id_bimap() {

    }

    explicit id_bimap(const id_bimap& other) {
        this->key_to_data = other.key_to_data;
        this->data_to_key = other.data_to_key;
    }

    ~id_bimap() {

    }

private:
    std::size_t m_size;
    avl_tree<Key, Mapped> m_key_to_data;
    avl_tree<Mapped, Key> m_data_to_key;
};

template<typename T>
using kchar_id_bimap = id_bimap<T, char>;

using string_id_bimap = id_bimap<std::string>;

#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
