#include "acid_map.hpp"
#include "tree_verifier.hpp"
#include "util.hpp"

#include "gtest/gtest.h"

using std::cout;
using std::endl;

class FilledMapTest : public ::testing::Test {
protected:
    FilledMapTest() = default;
    void SetUp() {
        for (int i = 0; i < n_; i++) {
            complex_object key = objects_generator_.next_value();
            complex_object value = objects_generator_.next_value();
            while (map_.contains(key)) {
                key = objects_generator_.next_value();
            }
            inserted_values_.emplace_back(key, value);
            map_.insert(std::make_pair(key, value));
        }
    }
    void TearDown() {
        map_.clear();
        inserted_values_.clear();
    }
    int n_ = 10000;
    std::vector<std::pair<complex_object, complex_object>> inserted_values_;
    polyndrom::acid_map<complex_object, complex_object> map_;
    complex_object_generator objects_generator_;
};
TEST_F(FilledMapTest, InsertExistingLvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        auto p = std::make_pair(key, objects_generator_.next_value());
        auto [it, flag] = map_.insert(p);
        EXPECT_FALSE(flag);
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertNotExistingLvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        complex_object value = objects_generator_.next_value();
        auto p = std::make_pair(key, value);
        auto [it, flag] = map_.insert(p);
        EXPECT_TRUE(flag);
        EXPECT_TRUE(it->first.has_copied());
        EXPECT_TRUE(it->second.has_copied());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertExistingRvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        auto p = std::make_pair(key, objects_generator_.next_value());
        auto [it, flag] = map_.insert(std::move(p));
        EXPECT_FALSE(flag);
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, InsertNotExistingRvalue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        complex_object value = objects_generator_.next_value();
        auto p = std::make_pair(key, value);
        auto [it, flag] = map_.insert(std::move(p));
        EXPECT_TRUE(flag);
        EXPECT_TRUE(it->first.has_moved());
        EXPECT_TRUE(it->second.has_moved());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EmplaceExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        complex_object new_value = make_unique_object(objects_generator_);
        auto key_params = std::make_tuple(key.num_, key.str_);
        auto value_params = std::make_tuple(new_value.num_, new_value.str_);
        auto [it, flag] = map_.emplace(std::piecewise_construct, key_params, value_params);
        EXPECT_FALSE(flag);
        EXPECT_FALSE(it->first.has_emplaced());
        EXPECT_FALSE(it->second.has_emplaced());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EmplaceNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        complex_object value = objects_generator_.next_value();
        auto key_params = std::make_tuple(key.num_, key.str_);
        auto value_params = std::make_tuple(value.num_, value.str_);
        auto [it, flag] = map_.emplace(std::piecewise_construct, key_params, value_params);
        EXPECT_TRUE(flag);
        EXPECT_TRUE(it->first.has_emplaced());
        EXPECT_TRUE(it->second.has_emplaced());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, TryEmplaceExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        complex_object new_value = make_unique_object(objects_generator_);
        auto [it, flag] = map_.try_emplace(key, new_value.num_, new_value.str_);
        EXPECT_FALSE(flag);
        EXPECT_FALSE(it->first.has_emplaced());
        EXPECT_FALSE(it->second.has_emplaced());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, TryEmplaceNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        complex_object value = objects_generator_.next_value();
        auto value_params = std::make_tuple(value.num_, value.str_);
        auto [it, flag] = map_.try_emplace(key, value.num_, value.str_);
        EXPECT_TRUE(flag);
        EXPECT_TRUE(it->first.has_copied());
        EXPECT_TRUE(it->second.has_emplaced());
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EraseExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto it = random_element(inserted_values_);
        auto& [key, value] = *it;
        int count = map_.erase(key);
        inserted_values_.erase(it);
        EXPECT_EQ(count, 1);
        EXPECT_EQ(prev_size - 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EraseNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        int count = map_.erase(key);
        EXPECT_EQ(count, 0);
        EXPECT_EQ(prev_size, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, EraseByIterator) {
    std::sort(inserted_values_.begin(), inserted_values_.end());
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto random_it = random_element(inserted_values_);
        auto it = map_.begin();
        std::advance(it, std::distance(inserted_values_.begin(), random_it));
        it = map_.erase(it);
        EXPECT_EQ(it->first, next(random_it)->first);
        EXPECT_EQ(it->second, next(random_it)->second);
        EXPECT_EQ(prev_size - 1, map_.size());
        inserted_values_.erase(random_it);
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, CheckLookupExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto& [key, value] = *random_element(inserted_values_);
        EXPECT_TRUE(map_.contains(key));
        EXPECT_EQ(map_.count(key), 1);
        EXPECT_NE(map_.find(key), map_.end());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, CheckLookupNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        complex_object key = make_unique_object(objects_generator_);
        EXPECT_FALSE(map_.contains(key));
        EXPECT_EQ(map_.count(key), 0);
        EXPECT_EQ(map_.find(key), map_.end());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, SubscriptOperatorForExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        EXPECT_EQ(map_[key], value);
        complex_object new_value = objects_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_[key], new_value);
        EXPECT_EQ(prev_size, map_.size());
        map_[key] = value;
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, SubscriptOperatorForNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        EXPECT_EQ(map_[key], complex_object());
        complex_object new_value = objects_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_[key], new_value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, AtForExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        auto& [key, value] = *random_element(inserted_values_);
        EXPECT_EQ(map_.at(key), value);
        complex_object new_value = objects_generator_.next_value();
        map_.at(key) = new_value;
        EXPECT_EQ(map_.at(key), new_value);
        EXPECT_EQ(prev_size, map_.size());
        map_.at(key) = value;
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, AtForNotExistingValue) {
    for (int i = 0; i < 1000; i++) {
        auto prev_size = map_.size();
        complex_object key = make_unique_object(objects_generator_);
        EXPECT_THROW(map_.at(key), std::out_of_range);
        complex_object new_value = objects_generator_.next_value();
        map_[key] = new_value;
        EXPECT_EQ(map_.at(key), new_value);
        EXPECT_EQ(prev_size + 1, map_.size());
    }
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, GetBeginIt) {
    auto it = map_.begin();
    auto& [key, value] = *std::min_element(inserted_values_.begin(), inserted_values_.end());
    EXPECT_EQ(it->first, key);
    EXPECT_EQ(it->second, value);
}
TEST_F(FilledMapTest, GetEndIt) {
    auto it = map_.begin();
    for (int i = 0; i < n_ - 1; i++) {
        ++it;
    }
    auto& [key, value] = *std::max_element(inserted_values_.begin(), inserted_values_.end());
    EXPECT_EQ(std::next(it), map_.end());
    EXPECT_EQ(it->first, key);
    EXPECT_EQ(it->second, value);
}
TEST_F(FilledMapTest, Clear) {
    map_.clear();
    for (auto& [key, value] : inserted_values_) {
        EXPECT_FALSE(map_.contains(key));
    }
    EXPECT_EQ(map_.size(), 0);
    EXPECT_TRUE(map_.empty());
    EXPECT_TRUE(polyndrom::verify_tree(map_));
}
TEST_F(FilledMapTest, IteratorDirectMove) {
    std::sort(inserted_values_.begin(), inserted_values_.end());
    auto it = map_.begin();
    for (auto& [key, value] : inserted_values_) {
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        ++it;
    }
}
TEST_F(FilledMapTest, IteratorReverseMove) {
    std::sort(inserted_values_.begin(), inserted_values_.end(), std::greater());
    auto it = map_.begin();
    for (int i = 0; i < n_ - 1; i++) {
        ++it;
    }
    for (auto& [key, value] : inserted_values_) {
        EXPECT_EQ(it->first, key);
        EXPECT_EQ(it->second, value);
        --it;
    }
}
