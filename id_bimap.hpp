//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include "utility.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>

//TODO check if integral type and if comparable, exception if not (this is ugly! - i mean the exception, the solution is nice)
//I might need my own exceptions
namespace bead {

    //constexpr auto variable = [](const auto& val) { return val; };

    template<class Mapped,
             class Key = std::size_t,
             class Compare = std::less<Key>,
             class Allocator = std::allocator<std::pair<const Key, Mapped>>>
    class id_bimap {
//Typedefs
    public:
        using key_type                  = Key;
        using mapped_type               = Mapped;
        using value_type                = std::pair<Key, Mapped>;
        using key_compare               = Compare;
        using allocator_type            = Allocator;
        using pointer                   = typename std::allocator_traits<Allocator>::pointer;
        using const_pointer             = typename std::allocator_traits<Allocator>::const_pointer;
        using reference                 = value_type &;
        using const_reference           = const value_type &;
        using size_type                 = std::size_t;
        using difference_type           = std::ptrdiff_t;//questionmark? TODO look this up
        using iterator                  = typename std::map<Key, Mapped, Compare, Allocator>::iterator;
        using const_iterator            = typename std::map<Key, Mapped, Compare, Allocator>::const_iterator;
        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

//constructors/destructor
    public:
        id_bimap();
        id_bimap(const id_bimap&);
        id_bimap(id_bimap&&) noexcept;
        id_bimap(std::initializer_list<Mapped>);
        ~id_bimap() = default;

//public member functions
    public:
        id_bimap& operator=(const id_bimap&);
        id_bimap& operator=(id_bimap&&) noexcept ;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        std::pair<const_iterator, bool> insert(const Mapped&); //TODO other signatures
        template<typename... Args>
        std::pair<const_iterator, bool> emplace(Args&&...);

        const Mapped& operator[](const Key&) const;
        const Key& operator[](const Mapped&) const;

        auto empty() const noexcept;
        auto size() const noexcept;
        auto clear();
        auto erase(const Key&);
        auto erase(const Mapped&);
        auto find(const Key&) const noexcept;
        auto find(const Mapped&) const noexcept;
        template<typename pred>
        auto find_if(pred) const;
        template<typename pred>
        auto delete_all(pred) -> void;

//private functions
    private:
        auto find_iter(const Mapped& m) const noexcept;
        auto reorder_map();

    private:
        std::size_t                                 m_size{};
        std::map<Key, Mapped, Compare, Allocator>   m_key_to_data;
        //std::map<Key, Mapped, Compare, Allocator>   m_data_to_key;
    };

//type aliases
    template<typename T>
    using kchar_id_bimap    = id_bimap<T, char>;

    using string_id_bimap   = id_bimap<std::string>;
}



//map constructor/destructor definitions
template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap() {
    map_constructor_check<Mapped, Key>();
}

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(const id_bimap& other)
    : m_key_to_data(other.m_key_to_data),
      m_size(other.m_size)
      { }

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(id_bimap&& other) noexcept
    : m_key_to_data(std::move(other.m_key_to_data)),
      m_size(std::move(other.m_size))
      { }

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>&
        bead::id_bimap<Mapped, Key, Compare, Allocator>::operator=(const id_bimap& other) {
    if (this != other) {
        this->m_key_to_data = other.m_key_to_data;
        this->m_size = other.m_size;
    }
    return *this;
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>&
        bead::id_bimap<Mapped, Key, Compare, Allocator>::operator=(id_bimap&& other) noexcept {
    if (this != other) {
        this->m_key_to_data = std::move(other.m_key_to_data);
        this->m_size = std::move(other.m_size);
    }
    return *this;
}


template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(std::initializer_list<Mapped> ilist) {
    map_constructor_check<Mapped, Key>();
    for (auto i : ilist) {
        this->insert(i);
    }
}

//map member function definitions

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator
        bead::id_bimap<Mapped, Key, Compare, Allocator>::begin() const noexcept {

    return m_key_to_data.begin();
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator
        bead::id_bimap<Mapped, Key, Compare, Allocator>::end() const noexcept {

    return m_key_to_data.end();
}

template<class Mapped, class Key, class Compare, class Allocator>
std::pair<typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator, bool>
        bead::id_bimap<Mapped, Key, Compare, Allocator>::insert(const Mapped &value) {

    Key next_key;

    for (auto it = begin(); it != end(); ++it) {
        if (it->second == value) {
            return { it, false };
        }
    }

    if (m_key_to_data.empty()) {
        next_key = 0;
    } else {
        next_key = m_key_to_data.rbegin()->first + 1;
    }

    return m_key_to_data.insert( { std::move(next_key), std::move(value) } );
}

template<class Mapped, class Key, class Compare, class Allocator>
template<typename... Args>
std::pair<typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator, bool>
bead::id_bimap<Mapped, Key, Compare, Allocator>::emplace(Args&&... args) {

    Key next_key;

    if (m_key_to_data.empty()) {
        next_key = 0;
    } else {
        next_key = m_key_to_data.rbegin()->first + 1;
    }
    return m_key_to_data.emplace(next_key, std::forward<Args>(args)...);
}

template<class Mapped, class Key, class Compare, class Allocator>
const Mapped& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Key& k) const {

    auto value = m_key_to_data.find(k);

    if ( value == m_key_to_data.end() ) {
        throw std::out_of_range("index out of range");
    }

    return value->second;
}

template<class Mapped, class Key, class Compare, class Allocator>
const Key& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Mapped& m) const {

    for (const auto&[key, mapped] : m_key_to_data) {
        if (mapped == m) {
            return key;
        }
    }
    throw std::domain_error("mapped value is not in map");
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::empty() const noexcept {
    return m_key_to_data.empty();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::size() const noexcept {
    return m_key_to_data.size();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::clear() {
    m_key_to_data.clear();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Key& key) {
    m_key_to_data.erase(key);
    reorder_map();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Mapped& mapped) {
    auto it = find_iter(mapped);
    if (it == end()) return;
    m_key_to_data.erase(it);
    reorder_map();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find_iter(const Mapped& m) const noexcept {
    for (auto it = begin(); it != end(); ++it) {
        if (it->second == m)
            return it;
    }
    return end();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find(const Key& key) const noexcept {
    try {
        Mapped m; //!!FONTOS!! nem szükséges, csak valamiért itt az assert vár plusz egy ctor-dtor hívást
        return m_key_to_data.find(key);
    } catch(...) {}
    return end();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find(const Mapped& mapped) const noexcept {
    return find_iter(mapped);
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::reorder_map() {
    std::map<Key, Mapped, Compare, Allocator> new_map;
    int new_key{};
    for (auto&& [key, mapped] : m_key_to_data) {
        auto value = std::make_pair(std::move(new_key++), std::move(mapped));
        new_map.insert( std::move(value) );
    }
    m_key_to_data = std::move(new_map);
}

template<class Mapped, class Key, class Compare, class Allocator>
template<typename pred>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find_if(pred p) const {
    auto npred = [&p](auto&& val){ return p(val.second); };
    return std::find_if(m_key_to_data.begin(), m_key_to_data.end(), npred);
}

template<class Mapped, class Key, class Compare, class Allocator>
template<typename pred>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::delete_all(pred p) -> void {

    auto npred = [&p](auto&& val){ return p(val.second); };
    /*
    for (auto it = m_key_to_data.begin();
         it != m_key_to_data.end();
         ++it) {

        if (npred(*it)) {
            m_key_to_data.erase(it);
        }
    }
    reorder_map()
    */
    for (auto it = m_key_to_data.begin();
            it != m_key_to_data.end();
            ++it) {

        if (npred(*it)) {
            m_key_to_data.erase(it);
            reorder_map();
            delete_all(p);
            break;
        }
    }
}


#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
