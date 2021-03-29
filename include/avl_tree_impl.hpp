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

template <class Key, class T, class Compare, class Allocator>
template <class V>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::avl_tree_node(V&& value, node_ptr left, node_ptr right,
																			  node_ptr parent, int8_t height)
	: value(std::forward<V>(value)), left(left), right(right), parent(parent), height(height) {}

template <class Key, class T, class Compare, class Allocator>
polyndrom::avl_tree<Key, T, Compare, Allocator>::avl_tree_node::~avl_tree_node() {
	value.~value_type();
}

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
		if (is_less(key, get_key(node))) {
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
void polyndrom::avl_tree<Key, T, Compare, Allocator>::destroy_subtree(node_ptr root) {
	if (root == nullptr) {
		return;
	}
	destroy_subtree(root->left);
	destroy_subtree(root->right);
	root->~avl_tree_node();
	node_allocator.deallocate(root, 1);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::rotate_left(node_ptr node) {
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

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::rotate_right(node_ptr node) {
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

template <class Key, class T, class Compare, class Allocator>
int8_t polyndrom::avl_tree<Key, T, Compare, Allocator>::height(node_ptr node) {
	if (node == nullptr) return 0;
	return node->height;
}

template <class Key, class T, class Compare, class Allocator>
void polyndrom::avl_tree<Key, T, Compare, Allocator>::update_height(node_ptr node) {
	if (node == nullptr) return;
	node->height = std::max(height(node->left), height(node->right)) + 1;
}

template <class Key, class T, class Compare, class Allocator>
int polyndrom::avl_tree<Key, T, Compare, Allocator>::balance_factor(node_ptr node) {
	if (node == nullptr) return 0;
	return height(node->left) - height(node->right);
}

template <class Key, class T, class Compare, class Allocator>
typename polyndrom::avl_tree<Key, T, Compare, Allocator>::node_ptr
polyndrom::avl_tree<Key, T, Compare, Allocator>::balance(node_ptr node) {
	int bf = balance_factor(node);
	if (bf == 2) {
		if (balance_factor(node->left) == -1) {
			node->left = rotate_left(node->left);
		}
		return rotate_right(node);
	} else if (bf == -2) {
		if (balance_factor(node->right) == 1) {
			node->right = rotate_right(node->right);
		}
		return rotate_left(node);
	}
	update_height(node);
	return node;
}

template <class Key, class T, class Compare, class Allocator>
void polyndrom::avl_tree<Key, T, Compare, Allocator>::balance_path(node_ptr node) {
	while (true) {
		if (balance_factor(node) == 0) {
			break;
		}
		node = balance(node);
		if (node->parent != nullptr) {
			node_side side = get_side(node);
			if (side == node_side::LEFT) {
				node->parent->left = node;
			} else {
				node->parent->right = node;
			}
		} else {
			root = node;
			break;
		}
		node = node->parent;
	}
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
