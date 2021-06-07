#include "acid_map.hpp"
#include "tree_verifier.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"

TEST(ConsistentMapTest, HardcodeInvalidate) {
    polyndrom::acid_map<int, int> map;
    for (int i = 1; i <= 10; i++) {
        map.emplace(i, i);
    }
    auto it1 = map.find(9);
    auto it2 = map.find(10);
    map.erase(it2);
    --it2;
    std::cout << it2->first << std::endl;
    /*EXPECT_EQ(it1, it2);
    ++it1;
    ++it2;
    EXPECT_EQ(it1, map.end());
    EXPECT_EQ(it2, map.end());*/
}

TEST(ConsistentMapTest, InvalidateAllDirect) {
    int n = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(n);
    for (int i = 0; i < n; i++) {
        its.push_back(map.emplace(i, i).first);
    }
    map.clear();
    for (auto it : its) {
        ++it;
        EXPECT_EQ(it, map.end());
    }
}

TEST(ConsistentMapTest, InvalidateAllReverse) {
    int n = 10000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(n);
    for (int i = 0; i < n; i++) {
        its.push_back(map.emplace(i, i).first);
    }
    map.clear();
    for (auto it : its) {
        --it;
        EXPECT_EQ(it, map.end());
    }
}

TEST(ConsistentMapTest, Invalidate) {
    int n = 10000;
    int m = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(n);
    for (int i = 0; i < n; i++) {
        map.emplace(i, i);
    }
    for (int i = 0; i < m; i++) {
        its.push_back(random_element(map));
    }
    for (auto it : its) {
        map.erase(it);
        EXPECT_FALSE(map.contains(it->first));
        ++it;
        EXPECT_TRUE(map.contains(it->first));
    }
}

TEST(ConsistentMapTest, RandomInvalidate) {
    int n = 10000;
    int m = 1000;
    int p = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(n);
    for (int i = 0; i < n; i++) {
        map.emplace(i, i);
    }
    for (int i = 0; i < m; i++) {
        its.push_back(random_element(map));
    }
    int_generator dir_generator(0, 1);
    int_generator step_generator(0, p);
    for (auto it : its) {
        int dir = dir_generator.next_value();
        int step = step_generator.next_value();
        for (int i = 0; i < step; i++) {
            if (dir == 0) {
                if (std::next(it) == map.end()) {
                    break;
                }
                ++it;
            } else {
                if (it == map.begin()) {
                    break;
                }
                --it;
            }
        }
        map.erase(it);
        EXPECT_FALSE(map.contains(it->first));
        auto it2 = it++;
        if (it == map.end()) {
            continue;
        }
        EXPECT_TRUE(map.contains(it->first));
        EXPECT_LT(it2->first, it->first);
    }
}