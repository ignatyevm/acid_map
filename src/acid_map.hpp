#pragma once

#include <tuple>
#include <ostream>
#include <iterator>

namespace polyndrom {

template <class Tree>
class tree_verifier;

template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class acid_map {
private:
    template <class Tree>
    friend
    class tree_verifier;
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
    acid_map(const allocator_type& allocator = allocator_type()) : node_allocator_(allocator) {}
    template <class K>
    iterator find(const K& key) {
        auto [parent, node] = find_node(root_, key);
        if (node == nullptr) {
            return end();
        }
        return make_iterator(node);
    }
    template <typename K>
    mapped_type& operator[](K&& key) {
        return try_emplace(std::forward<K>(key)).first->second;
    }
    mapped_type& at(const key_type& key) {
        auto [parent, node] = find_node(root_, key);
        if (node == nullptr) {
            throw std::out_of_range("Key does not exists");
        }
        return node->value_.second;
    }
    template <class K>
    bool contains(const K& key) const {
        return count(key) == 1;
    }
    template <class K>
    size_type count(const K& key) const {
        auto [parent, node] = find_node(root_, key);
        return static_cast<size_type>(node != nullptr);
    }
    template <class V>
    std::pair<iterator, bool> insert(V&& value) {
        const key_type& key = value.first;
        auto [parent, existing_node] = find_node(root_, key);
        if (existing_node != nullptr) {
            return std::make_pair(make_iterator(existing_node), false);
        }
        node_ptr node = construct_node(node_allocator_, std::forward<V>(value));
        insert_node(parent, node);
        return std::make_pair(make_iterator(node), true);
    }
    template <class ...Args>
    std::pair<iterator, bool> emplace(Args&& ...args) {
        node_ptr node = construct_node(node_allocator_, std::forward<Args>(args)...);
        auto [parent, existing_node] = find_node(root_, node->key());
        if (existing_node != nullptr) {
            destroy_node(node_allocator_, node);
            return std::make_pair(make_iterator(existing_node), false);
        }
        insert_node(parent, node);
        return std::make_pair(make_iterator(node), true);
    }
    template <class K, class ...Args>
    std::pair<iterator, bool> try_emplace(K&& key, Args&& ...args) {
        auto [parent, existing_node] = find_node(root_, key);
        if (existing_node != nullptr) {
            return std::make_pair(make_iterator(existing_node), false);
        }
        node_ptr node = construct_node(node_allocator_, std::piecewise_construct,
                                       std::forward_as_tuple(std::forward<K>(key)),
                                       std::forward_as_tuple(std::forward<Args>(args)...));
        insert_node(parent, node);
        return std::make_pair(make_iterator(node), true);
    }
    size_type erase(const key_type& key) {
        auto [parent, node] = find_node(root_, key);
        if (node == nullptr) {
            return 0;
        }
        erase_node(node);
        return 1;
    }
    iterator erase(iterator pos) {
        node_ptr next = next_node(pos.node_);
        erase_node(pos.node_);
        return make_iterator(next);
    }
    iterator begin() {
        if (root_ == nullptr) {
            return end();
        }
        return make_iterator(min_node(root_));
    }
    iterator end() {
        return make_iterator();
    }
    size_type size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }
    void clear() {
        destroy_subtree(root_);
        size_ = 0;
        root_ = nullptr;
    }
    ~acid_map() {
        clear();
    }
private:
    class avl_tree_iterator {
    public:
        friend acid_map;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<const Key, T>;
        using difference_type = size_t;
        using pointer = value_type*;
        using reference = value_type&;
        avl_tree_iterator(node_allocator_type& node_allocator) : node_allocator_(node_allocator) {}
        avl_tree_iterator(const avl_tree_iterator& other) : node_allocator_(other.node_allocator_), node_(other.node_) {
            update_ref_count(node_, 1);
        }
        avl_tree_iterator& operator=(const avl_tree_iterator& other) {
            if (this == &other) {
                return *this;
            }
            acquire_node(other.node_);
            return *this;
        }
        iterator& operator++() {
            acquire_node(next_node(node_));
            while (node_->is_deleted_) {
                acquire_node(next_node(node_));
            }
            return *this;
        }
        iterator operator++(int) {
            iterator other(*this);
            ++(*this);
            return other;
        }
        iterator& operator--() {
            acquire_node(prev_node(node_));
            while (node_->is_deleted_) {
                acquire_node(prev_node(node_));
            }
            return *this;
        }
        iterator operator--(int) {
            iterator other(*this);
            --(*this);
            return other;
        }
        value_type& operator*() {
            return node_->value_;
        }
        value_type* operator->() {
            return &node_->value_;
        }
        bool operator==(iterator other) const {
            return node_ == other.node_;
        }
        bool operator!=(iterator other) const {
            return node_ != other.node_;
        }
        ~avl_tree_iterator() {
            if (node_ == nullptr) {
                return;
            }
            release_node(node_);
        }
    private:
        avl_tree_iterator(node_allocator_type& node_allocator, node_ptr node) : node_allocator_(node_allocator), node_(node) {
            update_ref_count(node_, 1);
        }
        void acquire_node(node_ptr node) {
            node_ptr prev_node = node_;
            node_ = node;
            update_ref_count(node_, 1);
            release_node(prev_node);
        }
        void release_node(node_ptr node) {
            update_ref_count(node, -1);
            try_destroy_node(node_allocator_, node);
        }
        std::reference_wrapper<node_allocator_type> node_allocator_;
        node_ptr node_ = nullptr;
    };
    class avl_tree_node {
    public:
        template <class Tree>
        friend
        class tree_verifier;
        friend acid_map;
        template <class... Args>
        avl_tree_node(Args&& ... args) : value_(std::forward<Args>(args)...) {}
        ~avl_tree_node() = default;
    private:
        const key_type& key() {
            return value_.first;
        }
        node_ptr left_ = nullptr;
        node_ptr right_ = nullptr;
        node_ptr parent_ = nullptr;
        int8_t height_ = 1;
        int ref_count_ = 0;
        bool is_deleted_ = false;
        value_type value_;
    };
    void insert_node(node_ptr where, node_ptr node) {
        ++size_;
        if (root_ == nullptr) {
            root_ = node;
            update_ref_count(root_, 1);
            return;
        }
        auto [parent, _] = find_node(where, node->key());
        set_parent(node, parent);
        if (is_less(node->key(), parent->key())) {
            set_left_child(parent, node);
        } else {
            set_right_child(parent, node);
        }
        update_height(node->parent_);
        rebalance_path(node->parent_);
    }
    void erase_node(node_ptr node) {
        if (node->is_deleted_) {
            try_destroy_node(node_allocator_, node);
            return;
        }
        node_ptr parent = node->parent_;
        node_ptr replacement;
        node_ptr for_rebalance;
        if (node->left_ == nullptr || node->right_ == nullptr) {
            if (node->left_ != nullptr) {
                replacement = node->left_;
            } else {
                replacement = node->right_;
            }
            if (replacement != nullptr) {
                // replacement->parent_ = parent;
                set_parent(replacement, parent);
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = parent;
        } else {
            replacement = min_node(node->right_);
            node_ptr replacement_parent = replacement->parent_;
            // replacement->left_ = node->left_;
            set_left_child(replacement, node->left_);
            if (node->left_ != nullptr) {
                // node->left_->parent_ = replacement;
                set_parent(node->left_, replacement);
            }
            update_at_parent(parent, node, replacement);
            for_rebalance = replacement;
            if (node->right_ != replacement) {
                if (replacement->right_ != nullptr) {
                    // replacement->right_->parent_ = replacement->parent_;
                    set_parent(replacement->right_, replacement->parent_);
                }
                // replacement_parent->left_ = replacement->right_;
                set_left_child(replacement_parent, replacement->right_);
                // replacement->right_ = node->right_;
                set_right_child(replacement, node->right_);
                // node->right_->parent_ = replacement;
                set_parent(node->right_, replacement);
                for_rebalance = replacement_parent;
            }
            // replacement->parent_ = parent;
            set_parent(replacement, parent);
        }
        // update_ref_count(node->parent_, -1);
        // update_ref_count(replacement, -1);
        node->is_deleted_ = true;
        if (node == root_) {
            // root_ = replacement;
            update_root(replacement);
        }
        --size_;
        update_height(for_rebalance);
        rebalance_path(for_rebalance);
        try_destroy_node(node_allocator_, node);
    }
    static void set_parent(node_ptr node, node_ptr parent) {
        update_ref_count(node->parent_, -1);
        node->parent_ = parent;
        update_ref_count(parent, 1);
    }
    static void set_left_child(node_ptr parent, node_ptr node) {
        update_ref_count(parent->left_, -1);
        parent->left_ = node;
        update_ref_count(node, 1);
    }
    static void set_right_child(node_ptr parent, node_ptr node) {
        update_ref_count(parent->right_, -1);
        parent->right_ = node;
        update_ref_count(node, 1);
    }
    void update_root(node_ptr new_root) {
        if (root_ == new_root) {
            return;
        }
        update_ref_count(root_, -1);
        root_ = new_root;
        update_ref_count(root_, 1);
    }
    static void update_ref_count(node_ptr node, int count) {
        if (node == nullptr) {
            return;
        }
        node->ref_count_ += count;
    }
    template <class K1, class K2>
    inline bool is_less(const K1& lhs, const K2& rhs) const {
        return comparator_(lhs, rhs);
    }
    template <class K1, class K2>
    inline bool is_equal(const K1& lhs, const K2& rhs) const {
        return !is_less(lhs, rhs) && !is_less(rhs, lhs);
    }
    static inline bool is_left_child(node_ptr node) {
        return node->parent_->left_ == node;
    }
    static inline bool is_right_child(node_ptr node) {
        return node->parent_->right_ == node;
    }
    inline iterator make_iterator(node_ptr node = nullptr) {
        return avl_tree_iterator(node_allocator_, node);
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
    void update_at_parent(node_ptr parent, node_ptr old_node, node_ptr new_node) {
        if (parent == nullptr) {
            return;
        }
        if (is_left_child(old_node)) {
            // parent->left_ = new_node;
            set_left_child(parent, new_node);
        } else {
            // parent->right_ = new_node;
            set_right_child(parent, new_node);
        }
    }
    template <class... Args>
    static node_ptr construct_node(node_allocator_type node_allocator, Args&& ... args) {
        node_ptr node = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, node, std::forward<Args>(args)...);
        return node;
    }
    static void destroy_node(node_allocator_type node_allocator, node_ptr node) {
        std::allocator_traits<node_allocator_type>::destroy(node_allocator, node);
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator, node, 1);
    }
    static void try_destroy_node(node_allocator_type& node_allocator, node_ptr node) {
        if (node->is_deleted_ && node->ref_count_ == 0) {
            update_ref_count(node->parent_, -1);
            update_ref_count(node->left_, -1);
            update_ref_count(node->right_, -1);
            destroy_node(node_allocator, node);
        }
    }
    node_ptr rotate_left(node_ptr node) {
        /*node_ptr right_child = node->right_;
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
        return right_child;*/
        node_ptr right_child = node->right_;
        if (node->right_ != nullptr) {
            set_right_child(node, right_child->left_);
        }
        if (right_child->left_ != nullptr) {
            set_parent(right_child->left_, node);
        }
        set_left_child(right_child, node);
        set_parent(right_child, node->parent_);
        set_parent(node, right_child);
        update_height(node);
        update_height(right_child);
        return right_child;
    }
    node_ptr rotate_right(node_ptr node) {
        /*node_ptr left_child = node->left_;
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
        return left_child;*/
        node_ptr left_child = node->left_;
        if (node->left_ != nullptr) {
            set_left_child(node, left_child->right_);
        }
        if (left_child->right_ != nullptr) {
            set_parent(left_child->right_, node);
        }
        set_right_child(left_child, node);
        set_parent(left_child, node->parent_);
        set_parent(node, left_child);
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
    void update_height(node_ptr node) {
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
    node_ptr rebalance(node_ptr node) {
        int bf = balance_factor(node);
        if (bf == 2) {
            if (balance_factor(node->left_) == -1) {
                set_left_child(node, rotate_left(node->left_));
                // node->left_ = rotate_left(node->left_);
            }
            node = rotate_right(node);
        } else if (bf == -2) {
            if (balance_factor(node->right_) == 1) {
                // node->right_ = rotate_right(node->right_);
                set_right_child(node, rotate_right(node->right_));
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
                // node->parent_->left_ = node;
                set_left_child(node->parent_, node);
            } else {
                // node->parent_->right_ = node;
                set_right_child(node->parent_, node);
            }
            node = node->parent_;
        }
        // root_ = rebalance(root_);
        update_root(rebalance(root_));
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
    void destroy_subtree(node_ptr root) {
        if (root == nullptr) {
            return;
        }
        destroy_subtree(root->left_);
        destroy_subtree(root->right_);
        destroy_node(node_allocator_, root);
    }
    node_ptr root_ = nullptr;
    size_type size_ = 0;
    key_compare comparator_;
    mutable node_allocator_type node_allocator_;
};

} // polyndrom