#pragma once

#include <tuple>

template <class Key, class T, class Compare, class Allocator>
template <class K>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_iterator
polyndrom::avl_tree<Key, T, Compare, Allocator>::find(const K& key) const {
	auto [parent, node, side] = find_node(key);
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

template <class Key, class T, class Compare, class Allocator>
template <class V>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(V&& value, node_ptr left,
																			  node_ptr right, node_ptr parent)
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
	return get_side(node->parent, get_key(node));
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::min_node(node_ptr root) {
	while (root->left != nullptr) {
		root = root->left;
	}
	return root;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::max_node(node_ptr root) {
	while (root->right != nullptr) {
		root = root->right;
	}
	return root;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::nearest_parent_of(node_ptr node, node_side side) {
	while (node != nullptr) {
		if (node->parent != nullptr && get_side(node) == side) {
			return node->parent;
		}
		node = node->parent;
	}
	return nullptr;
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::prev_node(node_ptr node) {
	if (node->left != nullptr) {
		return max_node(node);
	}
	return nearest_parent_of(node, node_side::RIGHT);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::next_node(node_ptr node) {
	if (node->right != nullptr) {
		return min_node(node);
	}
	return nearest_parent_of(node, node_side::LEFT);
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
		if (is_less(key, node)) {
			node = node->left;
			side = node_side::LEFT;
		} else {
			node = node->right;
			side = node_side::RIGHT;
		}
	}
	return std::make_tuple(parent, node, side);
}

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
