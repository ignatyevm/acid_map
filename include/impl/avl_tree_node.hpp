#pragma once

#include <tuple>

template <class Key, class T, class Compare, class Allocator>
template <class V>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(V&& value, node_ptr left, node_ptr right,
                                                                              node_ptr parent, int8_t height)
    : value(std::forward<V>(value)), left(left), right(right), parent(parent), height(height) {}

template <class Key, class T, class Compare, class Allocator>
template <class K, class ...Args>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(K&& key, Args&& ...args, node_ptr left, node_ptr right,
                                                                              node_ptr parent, int8_t height)
    : value(std::piecewise_construct, std::forward_as_tuple<K>(key), std::forward_as_tuple<Args...>(args...)),
      left(left), right(right), parent(parent), height(height) {}

template <class Key, class T, class Compare, class Allocator>
template <class ...Args>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(V&& value, node_ptr left, node_ptr right,
                                                                              node_ptr parent, int8_t height)
    : value(std::forward<V>(value)), left(left), right(right), parent(parent), height(height) {}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::~avl_tree_node() {
    value.~value_type();
}