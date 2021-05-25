#include "acid_map.hpp"
#include "tree_verifier.hpp"
#include "util.hpp"

#include "gtest/gtest.h"

using std::cout;
using std::endl;

class FilledMapTest : public ::testing::Test {
protected:
    FilledMapTest() : values_generator_(0, 1000) {}
    void SetUp() {
        inserted_keys_.reserve(n_);
        inserted_values_.reserve(n_);
        for (int i = 0; i < n_; i++) {
            complex_key key = key_generator_.next_value();
            while (map_.contains(key)) {
                key = key_generator_.next_value();
            }
            inserted_keys_.push_back(key);
            inserted_values_.push_back(values_generator_.next_value());
            map_.insert(std::make_pair(inserted_keys_.back(), inserted_values_.back()));
        }
    }
    void TearDown() {
        map_.clear();
        inserted_keys_.clear();
        inserted_values_.clear();
    }
    int n_ = 10000;
    std::vector<complex_key> inserted_keys_;
    std::vector<int> inserted_values_;
    polyndrom::acid_map<complex_key, int> map_;
    complex_key_generator key_generator_;
    int_generator values_generator_;
};
TEST_F(FilledMapTest, InsertExistingLvalue) {
    int_generator index_generator(0, (int) inserted_keys_.size() - 1);
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto p = std::make_pair(*random_element(inserted_keys_), 1);
        complex_key& key = p.first;
        key.clear_flags();
        auto [it, flag] = map_.insert(p);
        EXPECT_TRUE(key.is_flags_clear());
        EXPECT_FALSE(flag);
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertNotExistingLvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto p = std::make_pair(key_generator_.next_value(), 1);
        complex_key& key = p.first;
        key.b_ = 2;
        key.clear_flags();
        auto [it, flag] = map_.insert(p);
        EXPECT_TRUE(it->first.has_copied());
        EXPECT_TRUE(flag);
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertExistingRvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto p = std::make_pair(*random_element(inserted_keys_), 1);
        complex_key& key = p.first;
        key.clear_flags();
        auto [it, flag] = map_.insert(std::move(p));
        EXPECT_TRUE(key.is_flags_clear());
        EXPECT_FALSE(flag);
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertNotExistingRvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto p = std::make_pair(key_generator_.next_value(), 1);
        complex_key& key = p.first;
        key.b_ = 2;
        key.clear_flags();
        complex_key key_copy = key;
        auto [it, flag] = map_.insert(std::move(p));
        EXPECT_TRUE(it->first.has_moved());
        EXPECT_TRUE(flag);
        EXPECT_EQ(it->first, key_copy);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EraseExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto it = random_element(inserted_keys_);
        complex_key& key = *it;
        auto prev_size = map_.size();
        int count = map_.erase(key);
        inserted_keys_.erase(it);
        EXPECT_EQ(count, 1);
        EXPECT_EQ(prev_size - 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EraseNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_key key = key_generator_.next_value();
        key.b_ = 2;
        int count = map_.erase(key);
        EXPECT_EQ(count, 0);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, CheckLookupExistingValue) {
    for (int i = 0; i < 1000; i++) {
        complex_key& key = *random_element(inserted_keys_);
        EXPECT_TRUE(map_.contains(key));
        EXPECT_EQ(map_.count(key), 1);
        EXPECT_NE(map_.find(key), map_.end());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, CheckLookupNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        complex_key key = key_generator_.next_value();
        key.b_ = 2;
        EXPECT_FALSE(map_.contains(key));
        EXPECT_EQ(map_.count(key), 0);
        EXPECT_EQ(map_.find(key), map_.end());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, SubscriptOperatorForExistingValue) {
    int_generator index_generator(0, (int) inserted_keys_.size() - 1);
    for (int i = 0; i < 1000; i++) {
        int index = index_generator.next_value();
        auto prev_size = map_.size();
        complex_key& key = inserted_keys_[index];
        EXPECT_EQ(map_[key], inserted_values_[index]);
        int new_value = values_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_[key], new_value);
        EXPECT_EQ(prev_size, map_.size());
        map_[key] = inserted_values_[index];
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, SubscriptOperatorForNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_key key = key_generator_.next_value();
        key.b_ = 2;
        EXPECT_EQ(map_[key], 0);
        int new_value = values_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_[key], new_value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, AtForExistingValue) {
    int_generator index_generator(0, (int) inserted_keys_.size() - 1);
    for (int i = 0; i < 1000; i++) {
        int index = index_generator.next_value();
        auto prev_size = map_.size();
        complex_key& key = inserted_keys_[index];
        EXPECT_EQ(map_.at(key), inserted_values_[index]);
        int new_value = values_generator_.next_value();
        map_.at(key) = new_value;
        EXPECT_EQ(map_.at(key), new_value);
        EXPECT_EQ(prev_size, map_.size());
        map_.at(key) = inserted_values_[index];
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, AtForNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_key key = key_generator_.next_value();
        key.b_ = 2;
        EXPECT_THROW(map_.at(key), std::out_of_range);
        int new_value = values_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_.at(key), new_value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}