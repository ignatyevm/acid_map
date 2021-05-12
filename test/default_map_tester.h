#pragma once

#include "acid_map.hpp"

#include <map>

#include "gtest/gtest.h"

namespace polyndrom {

using std::tie;

template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class default_map_tester {
private:
    using Map = polyndrom::acid_map<Key, T, Compare, Allocator>;
    using CorrectMap = std::map<Key, T, Compare, Allocator>;

    using testable_size_type = typename Map::size_type;
    using testable_iterator = typename Map::iterator;
    using correct_iterator = typename CorrectMap::iterator;
public:
    default_map_tester() {
        EXPECT_EQ(testable_map_.size(), 0);
        EXPECT_TRUE(testable_map_.empty());
    }
    template <class V>
    std::pair<testable_iterator, bool> insert(V&& value) {
        check_count(value.first);
        bool is_inserted_testable, is_inserted_correct;
        testable_iterator testable_it;
        correct_iterator correct_it;
        tie(testable_it, is_inserted_testable) = testable_map_.insert(std::forward<V>(value));
        tie(correct_it, is_inserted_correct) = correct_map_.insert(std::forward<V>(value));
        EXPECT_EQ(is_inserted_testable, is_inserted_correct);
        EXPECT_EQ(testable_map_.size(), correct_map_.size());
        EXPECT_EQ(value.first, testable_it->first);
        EXPECT_EQ(testable_it->first, correct_it->second);
        return std::make_pair(testable_it, is_inserted_testable);
    }
    template <class ...Args>
    std::pair<testable_iterator, bool> emplace(Args&& ...args) {
        bool is_inserted_testable, is_inserted_correct;
        testable_iterator testable_it;
        correct_iterator correct_it;
        tie(testable_it, is_inserted_testable) = testable_map_.emplace(std::forward<Args>(args)...);
        tie(correct_it, is_inserted_correct) = correct_map_.emplace(std::forward<Args>(args)...);
        EXPECT_EQ(is_inserted_testable, is_inserted_correct);
        EXPECT_EQ(testable_map_.size(), correct_map_.size());
        EXPECT_EQ(testable_it->first, correct_it->second);
        return std::make_pair(testable_it, is_inserted_testable);
    }
    template <class K, class ...Args>
    std::pair<testable_iterator, bool> try_emplace(K&& key, Args&& ...args) {
        check_count(key);
        bool is_inserted_testable, is_inserted_correct;
        testable_iterator testable_it;
        correct_iterator correct_it;
        tie(testable_it, is_inserted_testable) = testable_map_.try_emplace(std::forward<K>(key), std::forward<Args>(args)...);
        tie(correct_it, is_inserted_correct) = correct_map_.try_emplace(std::forward<K>(key), std::forward<Args>(args)...);
        EXPECT_EQ(is_inserted_testable, is_inserted_correct);
        EXPECT_EQ(testable_map_.size(), correct_map_.size());
        EXPECT_EQ(key, testable_it->first);
        EXPECT_EQ(testable_it->first, correct_it->second);
        return std::make_pair(testable_it, is_inserted_testable);
    }
    testable_size_type erase(const Key& key) {
        check_count(key);
        int testable_count = testable_map_.erase(key);
        int correct_count = correct_map_.erase(key);
        EXPECT_EQ(testable_count, correct_count);
        check_count(key);
    }
    testable_size_type erase(testable_iterator pos) {
        check_count(pos->first);
        testable_iterator testable_count = testable_map_.erase(pos);
        correct_iterator correct_count = correct_map_.erase(pos);
        EXPECT_EQ(testable_count, correct_count);
        check_count(pos->first);
    }
    template <class K>
    testable_iterator find(const K& key) {
        check_count(key);
        testable_iterator testable_it = testable_map_.find(key);
        correct_iterator correct_it = correct_map_.find(key);
        if (testable_map_.contains(key)) {
            EXPECT_EQ(testable_it->first, correct_it->first);
            EXPECT_EQ(testable_it->second, correct_it->second);
        }
        return testable_it;
    }
    template <typename K>
    T& operator[](K&& key) {
        check_count(key);
        if (!testable_map_.contains(key)) {
            T& testable_value = testable_map_[key];
            T& correct_value = correct_map_[key];
            EXPECT_EQ(testable_value, T());
            EXPECT_TRUE(testable_map_.contains(key));
        }
        EXPECT_EQ(testable_map_[key], correct_map_[key]);
        return testable_map_[key];
    }
    T& at(const Key& key) {
        check_count(key);
        if (!testable_map_.contains(key)) {
            EXPECT_THROW(correct_map_.at(key), std::out_of_range);
            EXPECT_THROW(testable_map_.at(key), std::out_of_range);
        } else {
            EXPECT_EQ(testable_map_.at(key), correct_map_.at(key));
        }
        return testable_map_.at(key);
    }
    template <class K>
    bool contains(const K& key) {
        check_count(key);
        bool is_contains = testable_map_.contains(key);
        EXPECT_TRUE((correct_map_.count(key) == 1 && is_contains) ||
                    (correct_map_.count(key) == 0 && !is_contains));
        return is_contains;
    }
    template <class K>
    testable_size_type count(const K& key) {
        check_count();
        return testable_map_.count(key);
    }
    testable_size_type size() {
        EXPECT_EQ(testable_map_.size(), correct_map_.size());
        return testable_map_.size();
    }
    bool empty() const {
        EXPECT_EQ(testable_map_.empty(), correct_map_.empty());
        return testable_map_.empty();
    }
    void clear() {
        ASSERT_EQ(testable_map_.size(), correct_map_.size());
        testable_map_.clear();
        correct_map_.clear();
        ASSERT_EQ(testable_map_.size(), correct_map_.size());
        ASSERT_TRUE(testable_map_.empty());
        ASSERT_TRUE(correct_map_.empty());
    }
    testable_iterator begin() const {
        testable_iterator testable_begin = testable_map_.begin();
        correct_iterator correct_begin = correct_map_.end();
        if (correct_map_.empty()) {
            EXPECT_EQ(testable_begin, testable_map_.end());
        } else {
            EXPECT_EQ(testable_begin->first, correct_begin->first);
            EXPECT_EQ(testable_begin->second, correct_begin->second);
        }
        return testable_begin;
    }
    testable_iterator end() const {
        if (correct_map_.empty()) {
            EXPECT_EQ(testable_map_.begin(), testable_map_.end());
        }
        return testable_map_.end();
    }
    ~default_map_tester() {
        clear();
    }
    template <class K>
    void check_count(const K& key) {
        int testable_count = testable_map_.count(key);
        int correct_count = correct_map_.count(key);
        ASSERT_EQ(testable_count, correct_count);
        ASSERT_EQ(testable_map_.size(), correct_map_.size());
    }
    Map& testable_map() {
        return testable_map_;
    }
private:
    Map testable_map_;
    CorrectMap correct_map_;
};

}