#pragma once

#include "fwd.hpp"
#include "map_node.hpp"

template <class Map>
class map_iterator {
private:
    friend Map;
    using map_type = Map;
    using node_ptr = typename Map::node_ptr;
    using read_lock = typename Map::read_lock;
    using write_lock = typename Map::write_lock;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = size_t;
    using value_type = typename Map::value_type;
    using pointer = value_type*;
    using reference = value_type&;
    map_iterator(map_type* map) {
        this->map = map;
        node = nullptr;
    }
    map_iterator(const map_iterator& other) {
        read_lock rlock(other.map->rw_mutex);
        map = other.map;
        node = other.node;
    }
    map_iterator& operator=(const map_iterator& other) {
        read_lock rlock(other.map->rw_mutex);
        if (node == other.node) {
            return *this;
        }
        map = other.map;
        node = other.node;
        return *this;
    }
    map_iterator& operator++() {
        read_lock rlock(map->rw_mutex);
        node = node.next();
        return *this;
    }
    map_iterator operator++(int) {
        map_iterator other(map, node);
        ++*this;
        return other;
    }
    map_iterator& operator--() {
        read_lock rlock(map->rw_mutex);
        node = node.prev();
        return *this;
    }
    map_iterator operator--(int) {
        map_iterator other(map, node);
        --*this;
        return other;
    }
    value_type& operator*() {
        read_lock rlock(map->rw_mutex);
        return node->value;
    }
    value_type* operator->() {
        read_lock rlock(map->rw_mutex);
        return &node->value;
    }
    bool operator==(map_iterator other) const {
        read_lock rlock(map->rw_mutex);
        return node == other.node;
    }
    bool operator!=(map_iterator other) const {
        read_lock rlock(map->rw_mutex);
        return node != other.node;
    }
private:
    map_iterator(map_type* map, node_ptr node) {
        read_lock rlock(map->rw_mutex);
        this->map = map;
        this->node = node;
    }
    map_type* map = nullptr;
    node_ptr node = nullptr;
};
