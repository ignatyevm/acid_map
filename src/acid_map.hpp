#pragma once

#include "map_node.hpp"
#include "map_iterator.hpp"

#include <tuple>
#include <ostream>
#include <iterator>

namespace polyndrom {

template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class acid_map {
private:
    friend map_iterator<acid_map<Key, T, Compare, Allocator>>;
    template <class Map>
    friend class map_iterator;
    template <class Tree>
    friend class tree_verifier;
    using self_type = acid_map<Key, T, Compare, Allocator>;
    using node_ptr = node_pointer<std::pair<const Key, T>, Allocator>;
    using node_allocator_type = typename node_ptr::allocator_type;
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = map_iterator<self_type>;
    acid_map(const allocator_type& allocator = allocator_type()) : node_allocator(allocator) {}
    template <class K>
    iterator find(const K& key) {
        auto [parent, node] = find_node(root, key);
        if (node == nullptr) {
            return end();
        }
        return iterator(node);
    }
    template <typename K>
    mapped_type& operator[](K&& key) {
        return try_emplace(std::forward<K>(key)).first->second;
    }
    mapped_type& at(const key_type& key) {
        auto [parent, node] = find_node(root, key);
        if (node == nullptr) {
            throw std::out_of_range("Key does not exists");
        }
        return node->value.second;
    }
    template <class K>
    bool contains(const K& key) const {
        return count(key) == 1;
    }
    template <class K>
    size_type count(const K& key) const {
        auto [parent, node] = find_node(root, key);
        return static_cast<size_type>(node != nullptr);
    }
    template <class V>
    std::pair<iterator, bool> insert(V&& value) {
        const key_type& key = value.first;
        auto [parent, existing_node] = find_node(root, key);
        if (existing_node != nullptr) {
            return std::make_pair(iterator(existing_node), false);
        }
        node_ptr node = node_ptr(node_allocator, std::forward<V>(value));
        insert_node(parent, node);
        return std::make_pair(iterator(node), true);
    }
    template <class ...Args>
    std::pair<iterator, bool> emplace(Args&& ...args) {
        node_ptr node(node_allocator, std::forward<Args>(args)...);
        auto [parent, existing_node] = find_node(root, node->key());
        if (existing_node != nullptr) {
            return std::make_pair(iterator(existing_node), false);
        }
        insert_node(parent, node);
        return std::make_pair(iterator(node), true);
    }
    template <class K, class ...Args>
    std::pair<iterator, bool> try_emplace(K&& key, Args&& ...args) {
        auto [parent, existing_node] = find_node(root, key);
        if (existing_node != nullptr) {
            return std::make_pair(iterator(existing_node), false);
        }
        node_ptr node = node_ptr(node_allocator, std::piecewise_construct,
                                                 std::forward_as_tuple(std::forward<K>(key)),
                                                 std::forward_as_tuple(std::forward<Args>(args)...));
        insert_node(parent, node);
        return std::make_pair(iterator(node), true);
    }
    size_type erase(const key_type& key) {
        auto [parent, node] = find_node(root, key);
        if (node == nullptr) {
            return 0;
        }
        erase_node(node);
        return 1;
    }
    iterator erase(iterator pos) {
        node_ptr next = pos.node.next();
        erase_node(pos.node);
        return iterator(next);
    }
    iterator begin() {
        if (root == nullptr) {
            return end();
        }
        return iterator(root.min());
    }
    iterator end() {
        return iterator();
    }
    size_type size() const {
        return map_size;
    }
    bool empty() const {
        return map_size == 0;
    }
    void clear() {
        auto it1 = begin();
        while (it1 != end()) {
            auto it2 = it1++;
            erase(it2);
        }
        root = nullptr;
    }
    ~acid_map() {
        root.force_destroy();
    }
private:
    template <class K>
    std::pair<node_ptr, node_ptr> find_node(node_ptr where, const K& key) const {
        node_ptr parent = nullptr;
        node_ptr node = where;
        while (true) {
            if (node == nullptr) {
                return std::make_pair(parent, node);
            }
            if (is_equal(node->key(), key)) {
                return std::make_pair(parent, node);
            }
            parent = node;
            if (is_less(key, node->key())) {
                node = node->left;
            } else {
                node = node->right;
            }
        }
    }
    void insert_node(node_ptr where, node_ptr node) {
        ++map_size;
        if (root == nullptr) {
            root = node;
            return;
        }
        auto [parent, _] = find_node(where, node->key());
        node->parent = parent;
        if (is_less(node->key(), parent->key())) {
            parent->left = node;
        } else {
            parent->right = node;
        }
        update_height(node->parent);
        rebalance_path(node->parent);
    }
    void erase_node(node_ptr node) {
        if (node == nullptr || node->is_deleted) {
            return;
        }
        node_ptr parent = node->parent;
        node_ptr replacement;
        node_ptr for_rebalance;
        if (node->left == nullptr || node->right == nullptr) {
            if (node->left != nullptr) {
                replacement = node->left;
            } else {
                replacement = node->right;
            }
            if (replacement != nullptr) {
                replacement->parent = parent;
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = parent;
        } else {
            replacement = node->right.min();
            node_ptr replacement_parent = replacement->parent;
            replacement->left = node->left;
            if (node->left != nullptr) {
                node->left->parent = replacement;
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = replacement;
            if (node->right != replacement) {
                if (replacement->right != nullptr) {
                    replacement->right->parent = replacement->parent;
                }
                replacement_parent->left = replacement->right;
                replacement->right = node->right;
                node->right->parent = replacement;
                for_rebalance = replacement_parent;
            }
            replacement->parent = parent;
        }
        node->left = nullptr;
        node->right = nullptr;
        node->is_deleted = true;
        if (node == root) {
            root = replacement;
        }
        --map_size;
        update_height(for_rebalance);
        rebalance_path(for_rebalance);
    }
    void update_at_parent(node_ptr parent, node_ptr old_node, node_ptr new_node) const {
        if (parent == nullptr) {
            return;
        }
        if (old_node->parent->left == old_node) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }
    }
    node_ptr rebalance(node_ptr node) {
        int bf = balance_factor(node);
        if (bf == 2) {
            if (balance_factor(node->left) == -1) {
                node->left = rotate_left(node->left);
            }
            node = rotate_right(node);
        } else if (bf == -2) {
            if (balance_factor(node->right) == 1) {
                node->right = rotate_right(node->right);
            }
            node = rotate_left(node);
        }
        update_height(node);
        return node;
    }
    node_ptr rebalance_path(node_ptr node) {
        if (node == nullptr) {
            return nullptr;
        }
        while (node != root) {
            bool pos = node->parent->left == node;
            node = rebalance(node);
            if (pos) {
                node->parent->left = node;
            } else {
                node->parent->right = node;
            }
            node = node->parent;
        }
        root = rebalance(root);
        return node;
    }
    node_ptr rotate_left(node_ptr node) {
        node_ptr right_child = node->right;
        if (node->right != nullptr) {
            node->right = right_child->left;
        }
        if (right_child->left != nullptr) {
            right_child->left->parent = node;
        }
        right_child->left = node;
        right_child->parent = node->parent;
        node->parent = right_child;
        update_height(node);
        update_height(right_child);
        return right_child;
    }
    node_ptr rotate_right(node_ptr node) {
        node_ptr left_child = node->left;
        if (node->left != nullptr) {
            node->left = left_child->right;
        }
        if (left_child->right != nullptr) {
            left_child->right->parent = node;
        }
        left_child->right = node;
        left_child->parent = node->parent;
        node->parent = left_child;
        update_height(node);
        update_height(left_child);
        return left_child;
    }
    int balance_factor(node_ptr node) const {
        if (node == nullptr) {
            return 0;
        }
        return height(node->left) - height(node->right);
    }
    int height(node_ptr node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->height;
    }
    void update_height(node_ptr node) {
        if (node != nullptr) {
            node->height = std::max(height(node->left), height(node->right)) + 1;
        }
    }
    template <class K1, class K2>
    inline bool is_less(const K1& lhs, const K2& rhs) const {
        return comparator(lhs, rhs);
    }
    template <class K1, class K2>
    inline bool is_equal(const K1& lhs, const K2& rhs) const {
        return !is_less(lhs, rhs) && !is_less(rhs, lhs);
    }
    node_ptr root = nullptr;
    size_type map_size = 0;
    key_compare comparator;
    node_allocator_type node_allocator;
};

} // polyndrom