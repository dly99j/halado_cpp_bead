//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include <type_traits>
#include "avl_tree.hpp"

//TODO check if integral type and if comparable, exception if not (this is ugly! - i mean the exception, the solution is nice)
//I might need my own exceptions
template<   typename Mapped,
            typename Key = std::size_t ,
            typename = typename std::enable_if_t< std::is_integral_v<Key> >,
            typename = typename std::enable_if_t< !std::is_same< Mapped, Key >::value > >
class id_bimap {
public:

    using mapped_type = Mapped;
    using key_type = Key;

    id_bimap() {

    }

    explicit id_bimap(const id_bimap& other) {
        this->key_to_data = other.key_to_data;
        this->data_to_key = other.data_to_key;
    }

    ~id_bimap() {

    }

private:
    avl_tree<Key, Mapped> key_to_data;
    avl_tree<Mapped, Key> data_to_key;
};

template<typename T>
using kchar_id_bimap = id_bimap<T, char>;

using string_id_bimap = id_bimap<std::string>;

#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
