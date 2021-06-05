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
        return *this;
    }
    node_pointer(const node_pointer& other) : allocator_(other.allocator_) {
        acquire(other);
    }
    node_pointer(allocator_type& allocator, map_node* node) : node_(node), allocator_(&allocator) {
        update_ref_count(1);
    }
    node_pointer& operator=(const node_pointer& other) {
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
        update_ref_count(-1);
        try_destroy();
        node_ = nullptr;
    }
    void destroy() {
        std::allocator_traits<allocator_type>::destroy(*allocator_, node_);
        std::allocator_traits<allocator_type>::deallocate(*allocator_, node_, 1);
        node_ = nullptr;
    }
    bool try_destroy() {
        if (node_ == nullptr || node_->ref_count_ != 0) {
            return false;
        }
        for (node_pointer node : {node_->parent_, node_->left_, node_->right_}) {
            node.update_ref_count(-1);
            node.try_destroy();
        }
        destroy();
        return true;
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
        destroy();
    }
    void update_ref_count(int n) {
        if (node_ == nullptr) {
            return;
        }
        node_->ref_count_ += n;
    }
    map_node* node_ = nullptr;
    allocator_type* allocator_ = nullptr;
};