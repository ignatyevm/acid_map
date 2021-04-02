#pragma once

#pragma once

#include <tuple>

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree() : root(nullptr), current_size(0), node_allocator() {}

template <class Key, class T, class Compare, class Allocator>
template <class InputIt>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree(InputIt first, InputIt last) : avl_tree() {
    for (InputIt it = first; it != last; ++it) {
        insert(*it);
    }
}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree(const avl_tree& other)
    : avl_tree(other.begin(), other.end()) {}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree(avl_tree&& other)
    : root(other.root), current_size(other.current_size), node_allocator(other.node_allocator) {
    other.root = nullptr;
    other.current_size = 0;
}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree(std::initializer_list<value_type> init)
    : avl_tree(init.begin(), init.end()) {}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::~avl_tree() {
    destroy_subtree(root);
}

template <class Key, class T, class Compare, class Allocator>
template <class V>
std::pair<typename polyndrom::avl_tree<Key, T, Compare, Allocator>::iterator, bool>
polyndrom::avl_tree<Key, T, Compare, Allocator>::insert(V&& value) {
    if (root == nullptr) {
        root = new (node_allocator.allocate(1)) avl_tree_node(std::forward<V>(value));
        return std::make_pair(avl_tree_iterator(root), true);
    }
    auto [parent, node, side] = find_node(root, value.first);
    if (node != nullptr) {
        return std::make_pair(avl_tree_iterator(node), false);
    }
    node = new (node_allocator.allocate(1)) avl_tree_node(std::forward<V>(value));
    node->parent = parent;
    if (side == node_side::LEFT) {
        node->parent->left = node;
    } else {
        node->parent->right = node;
    }
    update_height(node->parent);
    balance_path(node->parent);
    return std::make_pair(avl_tree_iterator(node), true);
}

template <class Key, class T, class Compare, class Allocator>
template <class K>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::find(const K& key) const {
    auto [parent, node, side] = find_node(root, key);
    if (node == nullptr) {
        return end();
    }
    return avl_tree_iterator(node);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::size_type
polyndrom::avl_tree<Key, T, Compare, Allocator>::size() const {
    return current_size;
}

template <class Key, class T, class Compare, class Allocator>
bool polyndrom::avl_tree<Key, T, Compare, Allocator>::empty() const {
    return size() == 0;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::begin() const {
    return avl_tree_iterator(min_node(root));
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::end() const {
    return avl_tree_iterator(nullptr);
}