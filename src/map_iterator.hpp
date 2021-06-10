#pragma once

#include "fwd.hpp"
#include "map_node.hpp"

template <class Map>
class map_iterator {
private:
    friend Map;
    using node_ptr = typename Map::node_ptr;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = size_t;
    using value_type = typename Map::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    map_iterator() = default;
    map_iterator(const map_iterator& other) : node(other.node) {}
    map_iterator& operator=(const map_iterator& other) {
        node = other.node;
        return *this;
    }
    map_iterator& operator++() {
        node = node.next();
        return *this;
    }
    map_iterator operator++(int) {
        map_iterator other(node);
        ++*this;
        return other;
    }
    map_iterator& operator--() {
        node = node.prev();
        return *this;
    }
    map_iterator operator--(int) {
        map_iterator other(node);
        --*this;
        return other;
    }
    value_type& operator*() {
        return node->value;
    }
    value_type* operator->() {
        return &node->value;
    }
    bool operator==(map_iterator other) const {
        return node == other.node;
    }
    bool operator!=(map_iterator other) const {
        return node != other.node;
    }
private:
    map_iterator(node_ptr node) : node(node) {}
    node_ptr node = nullptr;
};
