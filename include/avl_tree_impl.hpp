#pragma once

#include <tuple>

template <class Key, class T, class Compare, class Allocator>
template <class V>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(
	V&& value, node_ptr left, node_ptr right, node_ptr parent)
	: value(std::forward<V>(value)), left(left), right(right), parent(parent) {}

template <class Key, class T, class Compare, class Allocator>
template <class K1, class K2>
bool polyndrom::avl_tree<Key, T, Compare, Allocator>::is_less(const K1& lhs, const K2& rhs) {
	return Compare()(lhs, rhs);
}

template <class Key, class T, class Compare, class Allocator>
template <class K1, class K2>
bool polyndrom::avl_tree<Key, T, Compare, Allocator>::is_equal(const K1& lhs, const K2& rhs) {
	return !is_less(lhs, rhs) && !is_less(rhs, lhs);
}

template <class Key, class T, class Compare, class Allocator>
const typename polyndrom::avl_tree<Key, T, Compare, Allocator>::key_type&
polyndrom::avl_tree<Key, T, Compare, Allocator>::get_key(node_ptr node) {
	return node->value.first;
}

template <class Key, class T, class Compare, class Allocator>
template <class K>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_side
polyndrom::avl_tree<Key, T, Compare, Allocator>::get_side(node_ptr root, const K& key) {
	return is_less(key, get_key(root)) ? node_side::LEFT : node_side::RIGHT;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_side
polyndrom::avl_tree<Key, T, Compare, Allocator>::get_side(node_ptr root, node_ptr node) {
	return get_side(root, get_key(node));
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_side
polyndrom::avl_tree<Key, T, Compare, Allocator>::get_side(node_ptr node) {
	return get_side(node->parent, node);
}

template <class Key, class T, class Compare, class Allocator>
template <class K>
std::tuple<
	typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr,
	typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr,
	typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_side>
polyndrom::avl_tree<Key, T, Compare, Allocator>::find_node(node_ptr root, const K& key) {
	node_ptr parent = nullptr;
	node_ptr node = root;
	node_side side = node_side::UNKNOWN;
	while (node != nullptr) {
		if (is_equal(key, get_key(node))) {
			return std::make_tuple(parent, node, side);
		}
		parent = node;
		side = get_side(node, key);
		if (side == node_side::LEFT) {
			node = node->left;
		} else {
			node = node->right;
		}
	}
	return std::make_tuple(parent, node, side);
}