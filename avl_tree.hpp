//
// Created by dly99j on 2/10/22.
//

#ifndef HALADO_CPP_BEAD_AVL_TREE_HPP
#define HALADO_CPP_BEAD_AVL_TREE_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <optional>
#include "utility.hpp"
//TODO elegantly check template param if it is of right type (??)
//TODO move constructor?
template< typename Key, typename Data >
struct avl_node {
    Key                         key;
    Data                        data;
    int                         balance { 0 };
    std::shared_ptr< avl_node > left { nullptr };
    std::shared_ptr< avl_node > right { nullptr };

    avl_node(const Key& t_key, const Data& t_data) : key(t_key), data(t_data) {}
    avl_node() = default;
};

template<typename Key, typename Data>
using node_ptr = std::shared_ptr< avl_node< Key, Data > >;

//TODO better variable names and general code cleanup
//TODO do i need different constructors?
template< typename Key, typename Data >
class avl_tree {
public:
    void                                    insert(const Key& k, const Data& d);
    void                                    erase(const Key& k);
    std::optional< node_ptr<Key, Data> >    find(const Key& k);

private:    
    node_ptr< Key, Data > m_root;

    void balance_pp_p(node_ptr< Key, Data >& t, node_ptr< Key, Data >& r);
    void balance_pp_m(node_ptr< Key, Data >& t, node_ptr< Key, Data >& r);
    void balance_mm_p(node_ptr< Key, Data >& t, node_ptr< Key, Data >& l);
    void balance_mm_m(node_ptr< Key, Data >& t, node_ptr< Key, Data >& l);
    void left_sub_tree_grown(node_ptr< Key, Data >& t, bool& d);
    void right_sub_tree_grown(node_ptr< Key, Data >& t, bool& d);
    void avl_insert(node_ptr< Key, Data >& t, const Key& k, const Data& data, bool& d);
    void balance_pp_zero(node_ptr< Key, Data >& t, node_ptr< Key, Data >& r);
    void balance_pp(node_ptr< Key, Data >& t, bool& d);
    void left_sub_tree_shrunk(node_ptr< Key, Data >& t, bool& d);
    void right_sub_tree_shrunk(node_ptr< Key, Data >& t, bool& d);
    void avl_rem_min(node_ptr< Key, Data >& t, node_ptr< Key, Data >& minp, bool& d);
    void right_sub_tree_min_to_root(node_ptr< Key, Data >& t, bool& d);
    void avl_del_root(node_ptr< Key, Data >& t, bool& d);
    void avl_del(node_ptr< Key, Data >& t, const Key& k, bool& d);
    void avl_find(node_ptr< Key, Data >& t, node_ptr< Key, Data >& res, const Key& k, bool& d);

};

template<typename Key, typename Data>
void avl_tree<Key, Data>::insert(const Key &k, const Data &d) {
    bool b{};
    avl_insert(m_root, k, d, b);
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::erase(const Key &k) {
    bool d{};
    avl_del(m_root, k, d);
}

template<typename Key, typename Data>
std::optional< node_ptr<Key, Data> > avl_tree<Key, Data>::find(const Key &k) {
    node_ptr< Key, Data > res { nullptr };
    bool d{};
    avl_find(m_root, res, k, d);
    if (res != nullptr) {
        return res;
    }
    return std::nullopt;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_pp_p(node_ptr< Key, Data > &t,
                                       node_ptr< Key, Data > &r) {
    t->right = r->left;
    r->left = t;
    r->balance = 0;
    t->balance = 0;
    t = r;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_pp_m(node_ptr< Key, Data > &t,
                                       node_ptr< Key, Data > &r) {
    auto l = r->left;
    t->right = l->left;
    r->left = l->right;
    l->left = t;
    l->right = r;
    t->balance = ((l->balance + 1) / 2) * -1;
    r->balance = ((1 - l->balance) / 2);
    l->balance = 0;
    t = l;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_mm_p(node_ptr< Key, Data > &t,
                                       node_ptr< Key, Data > &l) {
    auto r = l->right;
    l->right = r->left;
    t->left = r->right;
    r->left = l;
    r->right = t;
    l->balance = ((r->balance + 1) / 2) * -1;
    t->balance = ((1 - r->balance) / 2);
    r->balance = 0;
    t = r;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_mm_m(node_ptr< Key, Data > &t,
                                       node_ptr< Key, Data > &l) {
    t->left = l->right;
    l->right = t;
    l->balance = 0;
    t->balance = 0;
    t = l;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::left_sub_tree_grown(node_ptr< Key, Data > &t, bool &d) {
    if (t->balance == -1) {
        auto l = t->left;
        if (l->balance == -1) {
            balance_mm_m(t, l);
        } else {
            balance_mm_p(t, l);
        }
        d = false;
    } else {
        t->balance--;
        d = t->balance < 0;
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::right_sub_tree_grown(node_ptr< Key, Data > &t, bool &d) {
    if (t->balance == 1) {
        auto r = t->right;
        if (r->balance == 1) {
            balance_pp_p(t, r);
        } else {
            balance_pp_m(t, r);
        }
        d = false;
    } else {
        t->balance++;
        d = t->balance > 0;
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::avl_insert(node_ptr<Key, Data> &t, const Key &k,
                                     const Data &data, bool &d) {
    if (t == nullptr) {
        t = std::make_shared<avl_node < Key, Data >>(k, data);
    } else if (k < t->key) {
        avl_insert(t->left, k, data, d);
        if (d) {
            left_sub_tree_grown(t, d);
        }
    } else if (k > t->key) {
        avl_insert(t->right, k, data, d);
        if (d) {
            right_sub_tree_grown(t, d);
        }
    } else {
        d = false;
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_pp_zero(node_ptr<Key, Data> &t, 
                                          node_ptr<Key, Data> &r) {
    t->right = r->left;
    r->left = t;
    t->balance = 1;
    r->balance = -1;
    t = r;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::balance_pp(node_ptr<Key, Data> &t, bool &d) {
    auto r = t->right;
    switch (r->balance) {
        case -1:
            balance_pp_m(t, r);
            break;
        case 0:
            balance_pp_zero(t, r);
            d = false;
            break;
        case 1:
            balance_pp_p(t, r);
            break;
        default:
            throw std::runtime_error(
                    std::string("ya fucked up somewhere bud. balance is out of range at balance_pp method"));
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::left_sub_tree_shrunk(node_ptr<Key, Data> &t, bool &d) {
    if (t->balance == 1) {
        balance_pp(t, d);
    } else {
        t->balance++;
        d = t->balance == 0;
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::right_sub_tree_shrunk(node_ptr<Key, Data> &t, bool &d) {
    if (t->balance == -1) {
        balance_pp(t, d);
    } else {
        t->balance--;
        d = t->balance == 0;
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::avl_rem_min(node_ptr<Key, Data> &t, node_ptr<Key, Data> &minp, bool &d) {
    if (t->left == nullptr) {
        minp = t;
        t = minp->right;
        minp->right = nullptr;
        d = true;
    } else {
        avl_rem_min(t->left, minp, d);
        if (d) {
            left_sub_tree_shrunk(t, d);
        }
    }
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::right_sub_tree_min_to_root(node_ptr<Key, Data> &t, bool &d) {
    auto p = std::make_shared< avl_node< Key, Data > >();
    avl_rem_min(t->right, p, d);
    p->left = t->left;
    p->right = t->right;
    p->balance = t->balance;
    t = p;
}

template<typename Key, typename Data>
void avl_tree<Key, Data>::avl_del_root(node_ptr<Key, Data> &t, bool &d) {
    if (t->left == nullptr) {
        auto p = t;
        t = p->right;
        d = true;
    } else if (t->right == nullptr) {
        auto p = t;
        t = p->left;
        d = true;
    } else if (t->left == nullptr and t->right == nullptr) {
        right_sub_tree_min_to_root(t, d);
        if (d) {
            right_sub_tree_shrunk(t, d);
        }
    }
}
template<typename Key, typename Data>
void avl_tree<Key, Data>::avl_del(node_ptr<Key, Data> &t, const Key &k, bool &d) {
    if (t == nullptr) {
        d = false;
    } else {
        if (k < t->key) {
            avl_del(t->left, k, d);
            if (d) {
                left_sub_tree_shrunk(t, d);
            }
        } else if (k > t->key) {
            avl_del(t->right, k, d);
            if (d) {
                right_sub_tree_shrunk(t, d);
            }
        } else if (k == t->key) {
            avl_del_root(t, d);
        }
    }
}
template<typename Key, typename Data>
void avl_tree<Key, Data>::avl_find(node_ptr<Key, Data> &t, node_ptr<Key, Data> &res,
                                   const Key &k, bool &d) {
    if (t == nullptr) {
        d = false;
    } else if (t->key == k) {
        res = t;
    } else if (t->key > k) {
        avl_find(t->right, res, k, d);
    } else {
        avl_find(t->left, res, k, d);
    }
}

#endif//HALADO_CPP_BEAD_AVL_TREE_HPP