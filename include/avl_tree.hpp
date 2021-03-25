#pragma once

namespace polyndrom {
template <class Key, class T, class Compare = std::less<Key>,
		  class Allocator = std::allocator<std::pair<const Key, T>>>
class avl_tree {
private:
	// forward decl
	struct avl_tree_node;
	struct avl_tree_iterator;
public:
	// public interface
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
	using iterator = avl_tree_iterator;
	avl_tree();
	template <typename InputIt>
	avl_tree(InputIt first, InputIt last);
	avl_tree(const avl_tree& other);
	avl_tree(avl_tree&& other);
	avl_tree(std::initializer_list<value_type> init);
	template <typename V>
	std::pair<iterator, bool> insert(V&& value);
	iterator erase(iterator pos);
	size_type erase(const key_type& key);
	template <typename K>
	iterator find(const K& key);
	size_type size() const;
	bool empty() const;
	~avl_tree();
private:
	// private interface
	using node_ptr = avl_tree_node*;
	using node_allocator_type = typename allocator_type::template rebind<avl_tree_node>::other;
	class avl_tree_node {
		template <class V>
		explicit avl_tree_node(V&& value, node_ptr left = nullptr, node_ptr right = nullptr, node_ptr parent = nullptr);
		~avl_tree_node();
		node_ptr parent;
		node_ptr left;
		node_ptr right;
		value_type value;
	};
	/*
	 * simple enum for denoting the node position relative another node
	 */
	enum class node_side {
		LEFT, RIGHT, UNKNOWN
	};
	/*
	 * compares 2 keys
	 */
	template <class K1, class K2>
	static inline bool is_less(const K1& lhs, const K2& rhs);
	/*
	 * checks for equality of 2 keys
	 */
	template <class K1, class K2>
	static inline bool is_equal(const K1& lhs, const K2& rhs);
	/*
	 * returns key stored in node
	 */
	static inline const key_type& get_key(node_ptr node);
	/*
	 * returns node_side::LEFT if 'key' is (or should be) in the left subtree of the 'root'
	 * returns node_side::RIGHT if 'key' is (or should be) in the right subtree of the 'root'
	 * returns node_side::UNKNOWN otherwise
	 */
	template <class K>
	static inline node_side get_side(node_ptr root, const K& key);
	/*
	 * just like in get_side(node_ptr, const K&)
	 */
	static inline node_side get_side(node_ptr root, node_ptr node);
	/*
	 * returns node_side::LEFT if 'node' is in the left subtree of the its parent
	 * returns node_side::RIGHT if 'node' is in the right subtree of the its parent
	 * returns node_side::UNKNOWN otherwise
	 */
	static inline node_side get_side(node_ptr node);
	/*
	 * returns node with min key in root
	 */
	static inline node_ptr min_node(node_ptr root);
	/*
	 * returns node with max key in root
	 */
	static inline node_ptr max_node(node_ptr root);
	/*
	 * returns some parent of 'node', that is get_side(some parent, 'node') == 'side'
	 */
	static inline node_ptr nearest_parent_of(node_ptr node, node_side side);
	/*
	 * returns prev node that get_key(prev node) < get_key('node') and get_key(prev node) is maximal
	 */
	static inline node_ptr prev(node_ptr node);
	/*
	 * returns next node that is get_key('node') < get_key(next node) and get_key(next node) is minimal
	 */
	static inline node_ptr next(node_ptr node);
	/*
	 * returns tuple<arg1, arg2, arg3>
	 * if the key is already in the tree with the root='root'
	 *   arg1 - parent of arg2
	 *   arg2 - node, contains 'key'
	 *   arg3 - side of arg2 relative to arg1
	 * else
	 *   arg1 - node for key insertion
	 *   arg2 - nullptr
	 *   arg3 - position for key insertion
	 */
	template <class K>
	static std::tuple<node_ptr, node_ptr, node_side> find_node(node_ptr root, const K& key);

	// private state
	node_ptr root;
	size_type current_size;
};
} // namespace polyndrom

#include <avl_tree_impl.hpp>
