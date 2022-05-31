//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_ID_BIMAP_HPP
#define HALADO_CPP_BEAD_ID_BIMAP_HPP

#include "utility.hpp"
#include <algorithm>
#include <map>
#include <vector>
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
        auto capacity() const noexcept;
        auto clear() noexcept;
        auto erase(const Key&);
        auto erase(const Mapped&);
        auto find(const Key&) const noexcept;
        auto find(const Mapped&) const noexcept;
        template<typename pred>
        auto find_if(pred) const;
        template<typename pred>
        auto delete_all(pred) -> void;
        auto next_index() const;
        auto is_contiguous() const noexcept;
        auto reserve(std::size_t);

//private functions
    private:
        auto find_iter(const Mapped& m) const noexcept;
        auto reorder_map();
        auto get_last_used_key() const noexcept;

    private:
        std::map<Key, Mapped, Compare, Allocator>   m_key_to_data;
        std::vector<Key>                            m_deleted_indices;
        std::size_t                                 m_unconstructed_reserved{};
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
          m_deleted_indices(other.m_deleted_indices),
          m_unconstructed_reserved(other.m_unconstructed_reserved)
        { }

template<class Mapped, class Key, class Compare, class Allocator>
bead::id_bimap<Mapped, Key, Compare, Allocator>::id_bimap(id_bimap&& other) noexcept
        : m_key_to_data(other.m_key_to_data),
          m_deleted_indices(other.m_deleted_indices),
          m_unconstructed_reserved(other.m_unconstructed_reserved)
        { }

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>&
        bead::id_bimap<Mapped, Key, Compare, Allocator>::operator=(const id_bimap& other) {
    if (this != other) {
        this->m_key_to_data = other.m_key_to_data;
        this->m_deleted_indices = other.m_deleted_indices;
        this->m_unconstructed_reserved = other.m_unconstructed_reserved;
    }
    return *this;
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>&
        bead::id_bimap<Mapped, Key, Compare, Allocator>::operator=(id_bimap&& other) noexcept {
    if (this != other) {
        this->m_key_to_data = std::move(other.m_key_to_data);
        this->m_deleted_indices = std::move(other.m_deleted_indices);
        this->m_unconstructed_reserved = std::move(other.m_unconstructed_reserved);
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

    for (auto it = m_key_to_data.begin(); it != m_key_to_data.end(); ++it) {
        if (std::find(m_deleted_indices.begin(),
                      m_deleted_indices.end(),
                      it->first) == m_deleted_indices.end()) {
            return it;
        }
    }
    return m_key_to_data.end();
}

template<class Mapped, class Key, class Compare, class Allocator>
typename bead::id_bimap<Mapped, Key, Compare, Allocator>::const_iterator
        bead::id_bimap<Mapped, Key, Compare, Allocator>::end() const noexcept {

    for (auto it = m_key_to_data.rbegin(); it != m_key_to_data.rend(); ++it) {
        if (std::find(m_deleted_indices.begin(),
                      m_deleted_indices.end(),
                      it->first) == m_deleted_indices.end()) {
            return it.base();
        }
    }
    return m_key_to_data.begin();
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

    if (!m_deleted_indices.empty()) {
        auto min = std::min_element(m_deleted_indices.begin(), m_deleted_indices.end());
        auto inserted = m_key_to_data.insert( {*min, value} );
        if (inserted.second) {
            --m_unconstructed_reserved;
        } else {
            m_key_to_data[*min] = value;
        }
        m_deleted_indices.erase(min);
        return { find_iter(value), true };
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

    Key next_key{};
    if (!m_deleted_indices.empty()) {
        auto min = std::min_element(m_deleted_indices.begin(), m_deleted_indices.end());
        next_key = *min;
        auto erased_num = m_key_to_data.erase(next_key);
        if (erased_num == 0) {
            --m_unconstructed_reserved;
        }
        m_deleted_indices.erase(min);
    } else if (m_key_to_data.empty()) {
        next_key = 0;
    } else {
        next_key = m_key_to_data.rbegin()->first + 1;
    }
    return m_key_to_data.emplace(next_key, std::forward<Args>(args)...);
}

template<class Mapped, class Key, class Compare, class Allocator>
const Mapped& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Key& k) const {

    auto value = m_key_to_data.find(k);

    if ( value == m_key_to_data.end() || std::find(m_deleted_indices.begin(),
                                                   m_deleted_indices.end(),
                                                   value->first) != m_deleted_indices.end() ) {
        throw std::out_of_range("index out of range");
    }

    return value->second;
}

template<class Mapped, class Key, class Compare, class Allocator>
const Key& bead::id_bimap<Mapped, Key, Compare, Allocator>::operator[](const Mapped& m) const {

    for (const auto&[key, mapped] : m_key_to_data) {
        if ( mapped == m && std::find(m_deleted_indices.begin(),
                                      m_deleted_indices.end(),
                                      key) == m_deleted_indices.end() ) {
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
    return m_key_to_data.size() - m_deleted_indices.size() + m_unconstructed_reserved;
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::capacity() const noexcept {
    return m_key_to_data.size() + m_unconstructed_reserved;
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::clear() noexcept {
    m_key_to_data.clear();
    m_deleted_indices.clear();
}
template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Key& key) {
    //m_key_to_data.erase(key);
    m_key_to_data[key].Mapped::~Mapped();
    m_deleted_indices.push_back(key);
    //reorder_map();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::erase(const Mapped& mapped) {
    auto it = find_iter(mapped);
    if (it == end()) return;
    auto key = it->first;
    m_key_to_data.erase(it);
    m_key_to_data[key].Mapped::~Mapped();
    m_deleted_indices.push_back(key);
    //reorder_map();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find_iter(const Mapped& m) const noexcept {
    for (auto it = begin(); it != end(); ++it) {
        if (it->second == m && std::find(m_deleted_indices.begin(),
                                         m_deleted_indices.end(),
                                         it->first) == m_deleted_indices.end())
            return it;
    }
    return end();
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::find(const Key& key) const noexcept {
    if ( std::find(m_deleted_indices.begin(),
                   m_deleted_indices.end(),
                   key) != m_deleted_indices.end()) {
        return end();
    }
    try {
        Mapped m; //!!FONTOS!! nem szükséges, csak valamiért itt az assert vár plusz egy ctor-dtor
                  //a második részfeladat
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
    auto npred = [&p, this](auto&& val){ return p(val.second) && std::find(m_deleted_indices.begin(),
                                                                           m_deleted_indices.end(),
                                                                           val.first) == m_deleted_indices.end(); };
    return std::find_if(m_key_to_data.begin(), m_key_to_data.end(), npred);
}

template<class Mapped, class Key, class Compare, class Allocator>
template<typename pred>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::delete_all(pred p) -> void {

    auto npred = [&p, this](auto&& val){ return p(val.second) &&
                                                (std::find(m_deleted_indices.begin(),
                                                           m_deleted_indices.end(),
                                                           val.first) == m_deleted_indices.end()); };
    for (auto it = m_key_to_data.begin();
            it != m_key_to_data.end();
            ++it) {

        if (npred(*it)) {
            m_key_to_data.erase(it);
            delete_all(p);
            break;
        }
    }
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::next_index() const {
    if (m_deleted_indices.empty()) {
        Key next_key;
        if (m_key_to_data.empty()) {
            next_key = 0;
        } else {
            next_key = m_key_to_data.rbegin()->first + 1;
        }
        return next_key;
    } else {
        return *std::min_element(m_deleted_indices.begin(), m_deleted_indices.end());
    }
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::is_contiguous() const noexcept {
    return m_deleted_indices.size() == 0;
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::get_last_used_key() const noexcept {
    if (m_key_to_data.empty()) {
        return static_cast<std::size_t>(0);
    } else {
        return m_key_to_data.rbegin()->first;
    }
}

template<class Mapped, class Key, class Compare, class Allocator>
auto bead::id_bimap<Mapped, Key, Compare, Allocator>::reserve(std::size_t n_size) {
    auto last_used_key = get_last_used_key();
    std::size_t non_used{};
    for (const auto& i : m_deleted_indices) {
        if (i > last_used_key) ++non_used;
    }
    auto cap = capacity();
    if (n_size < (cap - non_used)) return;
    else if (n_size >= (cap - non_used) && n_size < cap) {
        Key largest_key;
        for (auto i = cap - 1; i > n_size - 1; --i) {
            if (std::find(m_deleted_indices.begin(), m_deleted_indices.end(), i) != m_deleted_indices.end()) {
                if (m_key_to_data.find(i) != m_key_to_data.end()) {
                    m_key_to_data.erase(i);
                } else {
                    --m_unconstructed_reserved;
                }
                m_deleted_indices.erase(std::remove(m_deleted_indices.begin(),
                                                    m_deleted_indices.end(),
                                                    i), m_deleted_indices.end());
            }
        }
        return;
    }
    else if (n_size > cap) {

        for (auto i = cap; i < n_size; ++i) {
            m_deleted_indices.push_back(i);
            ++m_unconstructed_reserved;
        }
        return;
    }
}


#endif//HALADO_CPP_BEAD_ID_BIMAP_HPP
