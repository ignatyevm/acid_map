#pragma once

#include "fwd.hpp"

#include <atomic>

template <class V>
class node_pointer {
private:
    class map_node {
    public:
        template <class... Args>
        map_node(Args&& ... args) : value(std::forward<Args>(args)...) {}
        ~map_node() = default;
        const auto& key() {
            return value.first;
        }
        node_pointer left = nullptr;
        node_pointer right = nullptr;
        node_pointer parent = nullptr;
        int8_t height = 1;
        std::atomic_size_t ref_count = 0;
        std::atomic_bool is_deleted = false;
        V value;
    };
public:
    node_pointer() = default;
    template <class... Args>
    node_pointer(std::piecewise_construct_t, Args&&... args) {
        owned_node = new map_node(std::forward<Args>(args)...);
        owned_node->ref_count += 1;
    }
    node_pointer(std::nullptr_t) {}
    node_pointer& operator=(std::nullptr_t) {
        release();
        return *this;
    }
    node_pointer(const node_pointer& other) {
        acquire(other);
    }
    node_pointer& operator=(const node_pointer& other) {
        if (owned_node == other.owned_node) {
            return *this;
        }
        release();
        acquire(other);
        return *this;
    }
    map_node* operator->() {
        return owned_node;
    }
    bool operator==(node_pointer rhs) const {
        return owned_node == rhs.owned_node;
    }
    bool operator!=(node_pointer rhs) const {
        return owned_node != rhs.owned_node;
    }
    ~node_pointer() {
        release();
    }
    void acquire(const node_pointer& other) {
        owned_node = other.owned_node;
        if (owned_node != nullptr) {
            owned_node->ref_count += 1;
        }
    }
    void release() {
        if (owned_node != nullptr) {
            owned_node->ref_count -= 1;
            if (owned_node->ref_count == 0) {
                delete owned_node;
            }
            owned_node = nullptr;
        }
    }
    void force_destroy() {
        if (owned_node == nullptr) {
            return;
        }
        if (owned_node->left != nullptr) {
            owned_node->left.force_destroy();
        }
        if (owned_node->right != nullptr) {
            owned_node->right.force_destroy();
        }
        if (owned_node->left == nullptr && owned_node->right == nullptr) {
            owned_node->parent = nullptr;
            delete owned_node;
            owned_node = nullptr;
        }
    }
    node_pointer prev() {
        if (owned_node->is_deleted) {
            return nearest_not_deleted();
        }
        if (owned_node->left != nullptr) {
            return owned_node->left.max();
        }
        return nearest_right_ancestor();
    }
    node_pointer next() {
        if (owned_node->is_deleted) {
           return nearest_not_deleted();
        }
        if (owned_node->right != nullptr) {
            return owned_node->right.min();
        }
        return nearest_left_ancestor();
    }
    node_pointer min() {
        node_pointer node = *this;
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }
    node_pointer max() {
        node_pointer node = *this;
        while (node->right != nullptr) {
            node = node->right;
        }
        return node;
    }
    node_pointer nearest_left_ancestor() {
        node_pointer node = *this;
        while (node != nullptr) {
            if (node->parent != nullptr && node->parent->left == node) {
                return node->parent;
            }
            node = node->parent;
        }
        return nullptr;
    }
    node_pointer nearest_right_ancestor() {
        node_pointer node = *this;
        while (node != nullptr) {
            if (node->parent != nullptr && node->parent->right == node) {
                return node->parent;
            }
            node = node->parent;
        }
        return nullptr;
    }
    node_pointer nearest_not_deleted() {
        node_pointer node = *this;
        while (node != nullptr && node->is_deleted) {
            node = node->parent;
        }
        return node;
    }
    map_node* owned_node = nullptr;
    // map_type* map = nullptr;
};