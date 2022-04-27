//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include <type_traits>
#include <memory>
#include <unordered_map>
#include <stdexcept>

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
        using iterator          = typename std::unordered_map<Mapped, Key, Compare, Allocator>::iterator;
        using const_iterator    = typename std::unordered_map<Mapped, Key, Compare, Allocator>::const_iterator;
//constructors/destructor
    public:
        id_bimap();
        id_bimap(const id_bimap &other);//TODO move constructor
        id_bimap(std::initializer_list<Mapped>);
        ~id_bimap();

//public member functions
    public:
        const_iterator begin();
        const_iterator end();
        std::pair<const_iterator, bool> insert(const Mapped &value); //TODO other signatures

        const Mapped& operator[](const Key& k);
        const Key& operator[](const Mapped& m);

        auto empty();
        auto size();
        auto clear();
        auto erase(const Key& key);
        auto erase(const Mapped& mapped);
        auto find(const Key& key);
        auto find(const Mapped& mapped);

//private functions
    private:
        auto find_iter(const Mapped& m);

    private:
        std::size_t                                 m_size{};
        std::unordered_map<Key, Mapped, Compare, Allocator>   m_key_to_data;
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
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(const id_bimap &other) {
    map_constructor_check<Mapped, Key>();
    this->m_size = other.m_size;
    this->m_key_to_data = other.m_key_to_data;
    //this->m_data_to_key = other.m_data_to_key;
}

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(std::initializer_list<Mapped> l) {
    map_constructor_check<Mapped, Key>();
    for (auto i : l) {
        this->insert(i);
    }
}


template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap::~id_bimap() {
    //??? TODO do i need this?
}

//map member function definitions

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator bead::id_bimap<Mapped, Key, Compare, Allocator>::begin() {
    return m_key_to_data.begin();
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator bead::id_bimap<Mapped, Key, Compare, Allocator>::end() {
    return m_key_to_data.end();
}

template<class Mapped, class Key, class Compare, class Allocator>
std::pair<typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator, bool>
        bead::id_bimap<Mapped, Key, Compare, Allocator>::insert(const Mapped &value) {

    Key next_key;

    if (m_key_to_data.empty()) {
        next_key = 0;
    } else {
        next_key = m_key_to_data.rbegin()->first + 1;
    }

    return m_key_to_data.insert(next_key, value);
}
template<class Mapped, class Key, class Compare, class Allocator>
const Mapped& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Key& k) {

    if (m_key_to_data.find(k) == m_key_to_data.end()) {
        throw std::out_of_range("index out of range");
    }

    return m_key_to_data[k];
}

template<class Mapped, class Key, class Compare, class Allocator>
const Key& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Mapped& m) {

    bool exists {};
    Key to_return;
    for (auto[key, mapped] : m_key_to_data) {
        if (mapped == m) {
            exists = true;
            to_return = key;
        }
    }
    if (!exists) {
        throw std::domain_error("mapped value is not in map");
    }

    return to_return;
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::empty() {
    return m_key_to_data.empty();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::size() {
    return m_key_to_data.size();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::clear() {
    m_key_to_data.clear();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Key& key) {
    m_key_to_data.erase(key);
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Mapped& mapped) {
    auto it = find_iter(mapped);
    if (it == nullptr) return;
    m_key_to_data.erase(it);
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find_iter(const Mapped& m) {
    for (auto it = begin(); it != end(); ++it) {
        if (it->first == m)
            return it;
    }
    return nullptr;
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find(const Key& key) {
    try {
        return m_key_to_data.find(key);
    } catch(...) {}
    return nullptr;
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find(const Mapped& mapped) {
    return find_iter(mapped);
}


#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
