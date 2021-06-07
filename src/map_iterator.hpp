#pragma once

#include "fwd.hpp"
#include "map_node.hpp"

template <class Map>
class map_iterator {
private:
    friend Map;
    using map_type = Map;
    using node_ptr = typename Map::node_ptr;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = size_t;
    using value_type = typename Map::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    map_iterator() = default;
    map_iterator(const map_iterator& other) : node_(other.node_) {}
    map_iterator& operator=(const map_iterator& other) {
        node_ = other.node_;
        return *this;
    }
    map_iterator& operator++() {
        node_ = node_.next();
        return *this;
    }
    map_iterator operator++(int) {
        map_iterator other(node_);
        ++*this;
        return other;
    }
    map_iterator& operator--() {
        node_ = node_.prev();
        return *this;
    }
    map_iterator operator--(int) {
        map_iterator other(node_);
        --*this;
        return other;
    }
    value_type& operator*() {
        return node_->value_;
    }
    value_type* operator->() {
        return &node_->value_;
    }
    bool operator==(map_iterator other) const {
        return node_ == other.node_;
    }
    bool operator!=(map_iterator other) const {
        return node_ != other.node_;
    }
private:
    map_iterator(node_ptr node) : node_(node) {}
    node_ptr node_ = nullptr;
};
