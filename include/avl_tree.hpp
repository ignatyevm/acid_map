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

	// private state
	node_ptr root;
	size_type current_size;
};
