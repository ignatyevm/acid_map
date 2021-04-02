#pragma once

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::avl_tree_iterator(node_ptr node) : node(node) {}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::avl_tree_iterator(const avl_tree_iterator& other)
    : node(other.node) {}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator&
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator=(const avl_tree_iterator& other) {
    node = other.node;
    return *this;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator&
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator++() {
    node = next_node(node);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator++(int) {
    avl_tree_iterator cpy(*this);
    node = next_node(node);
    return cpy;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator&
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator--() {
    node = prev_node(node);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator--(int) {
    avl_tree_iterator cpy(*this);
    node = prev_node(node);
    return cpy;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::value_type&
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator*() {
    return node->value;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::value_type*
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator->() {
    return &(node->value);
}

template <class Key, class T, class Compare, class Allocator>
bool polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator==(const avl_tree_iterator& other) {
    return node == other.node;
}

template <class Key, class T, class Compare, class Allocator>
bool polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::operator!=(const avl_tree_iterator& other) {
    return node != other.node;
}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator::~avl_tree_iterator() = default;
