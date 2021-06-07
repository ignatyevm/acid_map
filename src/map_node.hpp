#pragma once

#include "fwd.hpp"

template <class V, class Allocator>
class node_pointer {
private:
    class map_node {
    public:
        template <class... Args>
        map_node(Args&& ... args) : value_(std::forward<Args>(args)...) {}
        ~map_node() = default;
        const auto& key() {
            return value_.first;
        }
        node_pointer left_ = nullptr;
        node_pointer right_ = nullptr;
        node_pointer parent_ = nullptr;
        int8_t height_ = 1;
        size_t ref_count_ = 0;
        bool is_deleted_ = false;
        V value_;
    };
public:
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<map_node>;
    template <class... Args>
    static node_pointer construct_node(allocator_type& allocator, Args&&... args) {
        map_node* node = std::allocator_traits<allocator_type>::allocate(allocator, 1);
        std::allocator_traits<allocator_type>::construct(allocator, node, std::forward<Args>(args)...);
        return node_pointer(allocator, node);
    }
    node_pointer() = default;
    node_pointer(std::nullptr_t) {}
    node_pointer& operator=(std::nullptr_t) {
        release();
        node_ = nullptr;
        allocator_= nullptr;
        return *this;
    }
    node_pointer(const node_pointer& other) : allocator_(other.allocator_) {
        acquire(other);
    }
    node_pointer(allocator_type& allocator, map_node* node) : node_(node), allocator_(&allocator) {
        update_ref_count(1);
    }
    node_pointer& operator=(const node_pointer& other) {
        if (node_ == other.node_) {
            return *this;
        }
        allocator_ = other.allocator_;
        release();
        acquire(other);
        return *this;
    }
    map_node* operator->() const {
        return node_;
    }
    bool operator==(node_pointer rhs) const {
        return node_ == rhs.node_;
    }
    bool operator!=(node_pointer rhs) const {
        return !(node_ == rhs.node_);
    }
    ~node_pointer() {
        release();
    }
    void make_deleted() const {
        node_->is_deleted_ = true;
    }
    void acquire(const node_pointer& other) {
        allocator_ = other.allocator_;
        node_ = other.node_;
        update_ref_count(1);
    }
    void release() {
        if (node_ == nullptr) {
            return;
        }
        update_ref_count(-1);
        if (node_->ref_count_ == 0) {
            destroy();
        }
    }
    void destroy() {
        std::allocator_traits<allocator_type>::destroy(*allocator_, node_);
        std::allocator_traits<allocator_type>::deallocate(*allocator_, node_, 1);
        node_ = nullptr;
        allocator_ = nullptr;
    }
    void force_destroy() {
        if (node_ == nullptr) {
            return;
        }
        if (node_->left_ != nullptr) {
            node_->left_.force_destroy();
        }
        if (node_->right_ != nullptr) {
            node_->right_.force_destroy();
        }
        if (node_->left_ == nullptr && node_->right_ == nullptr) {
            node_->parent_ = nullptr;
            destroy();
            node_ = nullptr;
            allocator_ = nullptr;
        }
    }
    void update_ref_count(int n) {
        if (node_ == nullptr) {
            return;
        }
        node_->ref_count_ += n;
    }
    bool is_left_child() {
        return node_->parent_->left_ == *this;
    }
    bool is_right_child() {
        return node_->parent_->right_ == *this;
    }
    node_pointer min() {
        node_pointer node = *this;
        while (node->left_ != nullptr) {
            node = node->left_;
        }
        return node;
    }
    node_pointer max() {
        node_pointer node = *this;
        while (node->right_ != nullptr) {
            node = node->right_;
        }
        return node;
    }
    node_pointer nearest_left_ancestor() {
        node_pointer node = *this;
        while (node != nullptr) {
            if (node->parent_ != nullptr && node.is_left_child()) {
                return node->parent_;
            }
            node = node->parent_;
        }
        return nullptr;
    }
    node_pointer nearest_right_ancestor() {
        node_pointer node = *this;
        while (node != nullptr) {
            if (node->parent_ != nullptr && node.is_right_child()) {
                return node->parent_;
            }
            node = node->parent_;
        }
        return nullptr;
    }
    node_pointer prev() {
        node_pointer node = *this;
        if (node->is_deleted_) {
            if (node->left_ == nullptr && node->parent_ == nullptr) {
                return node_pointer(nullptr);
            }
            if (node->left_ != nullptr) {
                return node->left_.prev();
            }
            if (node->parent_ != nullptr) {
                return node->parent_.prev();
            }
            return node;
        }
        if (node->left_ != nullptr) {
            return node->left_.max();
        }
        return node.nearest_right_ancestor();
    }
    node_pointer next() {
        node_pointer node = *this;
        if (node->is_deleted_) {
            if (node->right_ == nullptr && node->parent_ == nullptr) {
                return node_pointer(nullptr);
            }
            if (node->right_ != nullptr) {
                return node->right_.next();
            }
            if (node->parent_ != nullptr) {
                return node->parent_.next();
            }
            return node;
        }
        if (node->right_ != nullptr) {
            return node->right_.min();
        }
        return node.nearest_left_ancestor();
    }
    map_node* node_ = nullptr;
    allocator_type* allocator_ = nullptr;
};