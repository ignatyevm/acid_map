#include <tuple>

namespace polyndrom {

template <class Key, class T, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
class acid_map {
    class avl_tree_node;
    class avl_tree_iterator;
    using node_ptr = avl_tree_node*;
    using node_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<avl_tree_node>;
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
    using iterator = avl_tree_iterator;
    bool verify(bool verify_height = false) {
        return verify_node(root_, verify_height);
    }
    int height() {
        return deep_height(root_);
    }
    int deep_height(node_ptr node) {
        if (node == nullptr) {
            return -1;
        }
        return std::max(deep_height(node->left_), deep_height(node->right_)) + 1;
    }
    bool verify_node(node_ptr node, bool verify_height) {
        if (node == nullptr) {
            return true;
        }
        node_ptr parent = node->parent_;
        node_ptr left = node->left_;
        node_ptr right = node->right_;
        if (parent != nullptr) {
            if (is_left_child(node) && parent->left_ != node) {
                std::cout << "parent left node: "
                << parent->value_.first << " "
                << (parent->left_ == nullptr ? -1 : parent->left_->value_.first)
                << " " << node->value_.first << std::endl;
                return false;
            }
            if (is_right_child(node) && parent->right_ != node) {
                std::cout << "parent right node: "
                << parent->value_.first << " "
                << (parent->right_ == nullptr ? -1 : parent->right_->value_.first)
                << " " << node->value_.first << std::endl;
                return false;
            }
        }
        if (left != nullptr) {
            if (left->parent_ != node) {
                std::cout << "left left->parent node: "
                << left->value_.first << " "
                << left->parent_->value_.first << " "
                << node->value_.first << std::endl;
                return false;
            }
        }
        if (right != nullptr) {
            if (right->parent_ != node) {
                std::cout << "right right->parent node: "
                << right->value_.first << " "
                << right->parent_->value_.first << " "
                << node->value_.first << std::endl;
                return false;
            }
        }
        int lheight = deep_height(node->left_);
        int rheight = deep_height(node->right_);
        int bf = lheight - rheight;
        if (verify_height && (bf > 1 || bf < -1)) {
            std::cout << "node lh rh " << node->value_.first << " " << lheight << " " << rheight << std::endl;
            return false;
        }
        return verify_node(left, verify_height) && verify_node(right, verify_height);
    }
    acid_map(const allocator_type& allocator = allocator_type()) : node_allocator_(allocator) {}
    template <class K>
    iterator find(const K& key) const {
        auto [parent, node] = find_node(root_, key);
        if (node == nullptr) {
            return end();
        }
        return avl_tree_iterator(node);
    }
    template <class K>
    bool contains(const K& key) const {
        return find(key) != end();
    }
    template <class V>
    std::pair<iterator, bool> insert(V&& value) {
        if (root_ == nullptr) {
            root_ = construct_node(std::forward<V>(value));
            ++size_;
            return std::make_pair(avl_tree_iterator(root_), true);
        }
        const key_type& key = value.first;
        auto [parent, node] = find_node(root_, key);
        if (node != nullptr) {
            return std::make_pair(avl_tree_iterator(node), false);
        }
        node = construct_node(std::forward<V>(value));
        node->parent_ = parent;
        if (is_less(key, parent->key())) {
            parent->left_ = node;
        } else {
            parent->right_ = node;
        }
        update_height(node->parent_);
        rebalance_path(node->parent_);
        ++size_;
        return std::make_pair(avl_tree_iterator(node), false);
    }
    size_type erase(const key_type& key) {
        auto [parent, node] = find_node(root_, key);
        if (node == nullptr) {
            return 0;
        }
        node_ptr replacement;
        node_ptr for_rebalance;
        if (node->left_ == nullptr || node->right_ == nullptr) {;
            if (node->left_ != nullptr) {
                replacement = node->left_;
            } else {
                replacement = node->right_;
            }
            if (replacement != nullptr) {
                replacement->parent_ = parent;
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = parent;
        } else {
            replacement = min_node(node->right_);
            node_ptr replacement_parent = replacement->parent_;
            replacement->left_ = node->left_;
            if (node->left_ != nullptr) {
                node->left_->parent_ = replacement;
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = replacement;
            if (node->right_ != replacement) {
                if (replacement->right_ != nullptr) {
                    replacement->right_->parent_ = replacement->parent_;
                }
                replacement_parent->left_ = replacement->right_;
                replacement->right_ = node->right_;
                node->right_->parent_ = replacement;
                for_rebalance = replacement_parent;
            }
            replacement->parent_ = parent;
        }
        if (node == root_) {
            root_ = replacement;
        }
        --size_;
        update_height(for_rebalance);
        rebalance_path(for_rebalance);
        destroy_node(node);
        return 1;
    }
    iterator begin() const {
        if (root_ == nullptr) {
            return end();
        }
        return avl_tree_iterator(min_node(root_));
    }
    iterator end() const {
        return avl_tree_iterator();
    }
    size_type size() const {
        return size_;
    }
    bool empty() const {
        return size_ != 0;
    }
    ~acid_map() {
        destroy_subtree(root_);
    }
private:
    class avl_tree_iterator {
        friend acid_map;
    public:
        avl_tree_iterator() = default;
        iterator& operator++() {
            node_ = next_node(node_);
            return *this;
        }
        iterator operator++(int) {
            iterator other(node_);
            node_ = next_node(node_);
            return other;
        }
        iterator& operator--() {
            node_ = prev_node(node_);
            return *this;
        }
        iterator operator--(int) {
            iterator other(node_);
            node_ = prev_node(node_);
            return other;
        }
        value_type& operator*() {
            return node_->value_;
        }
        value_type* operator->() {
            return node_->value_;
        }
        bool operator==(iterator other) {
            return node_ == other.node_;
        }
        bool operator!=(iterator other) {
            return node_ != other.node_;
        }
    private:
        avl_tree_iterator(node_ptr node) : node_(node) {}
        node_ptr node_ = nullptr;
    };
    class avl_tree_node {
        friend acid_map;
    public:
        template <class V>
        avl_tree_node(V&& value) : value_(std::forward<V>(value)) {}
        ~avl_tree_node() = default;
    private:
        const key_type& key() {
            return value_.first;
        }
        node_ptr left_ = nullptr;
        node_ptr right_ = nullptr;
        node_ptr parent_ = nullptr;
        int8_t height_ = 1;
        value_type value_;
    };
    template <class K1, class K2>
    inline bool is_less(K1 lhs, K2 rhs) const {
        return comparator_(lhs, rhs);
    }
    template <class K1, class K2>
    inline bool is_equal(K1 lhs, K2 rhs) const {
        return !is_less(lhs, rhs) && !is_less(rhs, lhs);
    }
    static inline bool is_left_child(node_ptr node) {
        return node->parent_->left_ == node;
    }
    static inline bool is_right_child(node_ptr node) {
        return node->parent_->right_ == node;
    }
    template <class K>
    std::pair<node_ptr, node_ptr> find_node(node_ptr root, const K& key) const {
        node_ptr parent = nullptr;
        node_ptr node = root;
        while (true) {
            if (node == nullptr) {
                return std::make_pair(parent, node);
            }
            if (is_equal(node->key(), key)) {
                return std::make_pair(parent, node);
            }
            parent = node;
            if (is_less(key, node->key())) {
                node = node->left_;
            } else {
                node = node->right_;
            }
        }
    }
    void update_at_parent(node_ptr parent, node_ptr old_node, node_ptr new_node) const {
        if (parent == nullptr) {
            return;
        }
        if (is_left_child(old_node)) {
            parent->left_ = new_node;
        } else {
            parent->right_ = new_node;
        }
    }
    template <class V>
    node_ptr construct_node(V&& value) const {
        node_ptr node = std::allocator_traits<node_allocator_type>::allocate(node_allocator_, 1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator_, node, std::forward<V>(value));
        return node;
    }
    void destroy_node(node_ptr node) const {
        std::allocator_traits<node_allocator_type>::destroy(node_allocator_, node);
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator_, node, 1);
    }
    node_ptr rotate_left(node_ptr node) const {
        node_ptr right_child = node->right_;
        if (node->right_ != nullptr) {
            node->right_ = right_child->left_;
        }
        if (right_child->left_ != nullptr) {
            right_child->left_->parent_ = node;
        }
        right_child->left_ = node;
        right_child->parent_ = node->parent_;
        node->parent_ = right_child;
        update_height(node);
        update_height(right_child);
        return right_child;
    }
    node_ptr rotate_right(node_ptr node) const {
        node_ptr left_child = node->left_;
        if (node->left_ != nullptr) {
            node->left_ = left_child->right_;
        }
        if (left_child->right_ != nullptr) {
            left_child->right_->parent_ = node;
        }
        left_child->right_ = node;
        left_child->parent_ = node->parent_;
        node->parent_ = left_child;
        update_height(node);
        update_height(left_child);
        return left_child;
    }
    int height(node_ptr node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->height_;
    }
    void update_height(node_ptr node) const {
        if (node != nullptr) {
            node->height_ = std::max(height(node->left_), height(node->right_)) + 1;
        }
    }
    int balance_factor(node_ptr node) const {
        if (node == nullptr) {
            return 0;
        }
        return height(node->left_) - height(node->right_);
    }
    node_ptr rebalance(node_ptr node) const {
        int bf = balance_factor(node);
        if (bf == 2) {
            if (balance_factor(node->left_) == -1) {
                node->left_ = rotate_left(node->left_);
            }
            node = rotate_right(node);
        } else if (bf == -2) {
            if (balance_factor(node->right_) == 1) {
                node->right_ = rotate_right(node->right_);
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
        while (node != root_) {
            bool pos = true;
            if (is_right_child(node)) {
                pos = false;
            }
            node = rebalance(node);
            if (pos) {
                node->parent_->left_ = node;
            } else {
                node->parent_->right_ = node;
            }
            node = node->parent_;
        }
        root_ = rebalance(root_);
        return node;
    }
    static node_ptr min_node(node_ptr root) {
        while (root->left_ != nullptr) {
            root = root->left_;
        }
        return root;
    }
    static node_ptr max_node(node_ptr root) {
        while (root->right_ != nullptr) {
            root = root->right_;
        }
        return root;
    }
    static node_ptr nearest_left_ancestor(node_ptr node) {
        while (node != nullptr) {
            if (node->parent_ != nullptr && is_left_child(node)) {
                return node->parent_;
            }
            node = node->parent_;
        }
        return nullptr;
    }
    static node_ptr nearest_right_ancestor(node_ptr node) {
        while (node != nullptr) {
            if (node->parent_ != nullptr && is_right_child(node)) {
                return node->parent_;
            }
            node = node->parent_;
        }
        return nullptr;
    }
    static node_ptr prev_node(node_ptr node) {
        if (node->left_ != nullptr) {
            return max_node(node->left_);
        }
        return nearest_right_ancestor(node);
    }
    static node_ptr next_node(node_ptr node) {
        if (node->right_ != nullptr) {
            return min_node(node->right_);
        }
        return nearest_left_ancestor(node);
    }
    void destroy_subtree(node_ptr root) const {
        if (root == nullptr) {
            return;
        }
        destroy_subtree(root->left_);
        destroy_subtree(root->right_);
        destroy_node(root);
    }
    node_ptr root_ = nullptr;
    size_type size_ = 0;
    key_compare comparator_;
    mutable node_allocator_type node_allocator_;
};
} // polyndrom