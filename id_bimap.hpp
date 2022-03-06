//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include <type_traits>
#include "avl_tree.hpp"

//TODO check if integral type and if comparable, exception if not (this is ugly! - i mean the exception, the solution is nice)
//I might need my own exceptions
namespace bead {

    template<class Mapped,
             class Key = std::size_t,
             class Compare = std::less<Key>,
             class Allocator = std::allocator<std::pair<const Key, Mapped>>>
    class id_bimap {
//Typedefs
    public:
        using mapped_type       = Mapped;
        using key_type          = Key;
        using value_type        = std::pair<Key, Mapped>;
        using size_type         = std::size_t;
        using difference_type   = std::ptrdiff_t;//questionmark? TODO look this up
        using key_compare       = Compare;
        using allocator_type    = Allocator;
        using reference         = value_type &;
        using const_reference   = const value_type &;
        using pointer           = typename std::allocator_traits<Allocator>::pointer;
        using const_pointer     = typename std::allocator_traits<Allocator>::const_pointer;
        //TODO other member types

    public:
//iterator
        struct iterator {
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = std::pair<Key, Mapped>;
            using pointer           = typename std::allocator_traits<Allocator>::pointer;
            using reference         = typename std::allocator_traits<Allocator>::const_pointer;

            explicit    iterator(pointer ptr);
            reference   operator*() const;
            pointer     operator->();
            iterator&   operator++();
            const       iterator operator++(int);
            bool        operator==(const iterator& rhs);
            bool        operator!=(const iterator& rhs);

        private:
            pointer m_ptr;
        };
//constructors/destructor
    public:
        id_bimap();
        id_bimap(const id_bimap &other);//TODO move constructor
        ~id_bimap();

//public member functions
    public:
        iterator begin();
        iterator end();
        //std::pair<> insert(const Mapped &value); //TODO other signatures

//private functions
    private:
        std::size_t             m_size{};
        avl_tree<Key, Mapped>   m_key_to_data;
        avl_tree<Mapped, Key>   m_data_to_key;
    };

//type aliases
    template<typename T>
    using kchar_id_bimap    = id_bimap<T, char>;

    using string_id_bimap   = id_bimap<std::string>;
}

//iterator definitions
template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::iterator(id_bimap::pointer ptr) : m_ptr(ptr) {
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::reference bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator*() const {
    return *m_ptr;
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::pointer bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator->() {
    return m_ptr;
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator &bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator++() {
    ++m_ptr;
    return *this;
}

template<class Mapped, class Key, class Compare, class Allocator>
const typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator++(int) {
    auto temp = *this;
    ++m_ptr;
    return temp;
}

template<class Mapped, class Key, class Compare, class Allocator>
bool bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator==(
        const bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator &rhs) {
    return this->m_ptr == rhs.m_ptr;
}

template<class Mapped, class Key, class Compare, class Allocator>
bool bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator::operator!=(
        const bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator &rhs) {
    return this->m_ptr != rhs.m_ptr;
}

//map constructor/destructor definitions
template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap() {
    map_constructor_check<Mapped, Key>();
}

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(const id_bimap &other) {
    map_constructor_check<Mapped, Key>();
    this->m_size = other.m_size;
    this->m_key_to_data = other.m_key_to_data;
    this->m_data_to_key = other.m_data_to_key;
}

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap::~id_bimap() {
    //??? TODO do i need this?
}

//map member function definitions

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator bead::id_bimap<Mapped, Key, Compare, Allocator>::begin() {
    return bead::id_bimap::iterator(nullptr);
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::iterator bead::id_bimap<Mapped, Key, Compare, Allocator>::end() {
    return bead::id_bimap::iterator(nullptr);
}
/*
template<class Mapped, class Key, class Compare, class Allocator>
typename id_bimap<Mapped, Key, Compare, Allocator>::value_type id_bimap<Mapped, Key, Compare, Allocator>::insert(const Mapped &value) {
    if (m_data_to_key.find(value) != std::nullopt) {

    }
    const auto largest = m_key_to_data.find_max_key();
    Key new_key;
    if (largest.has_value()) {
        new_key = largest.value() + 1;
    } else {
        new_key = 0;
    }
    m_key_to_data.insert(new_key, value);
    m_data_to_key.insert(value, new_key);
    ++m_size;
}
*/



#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
