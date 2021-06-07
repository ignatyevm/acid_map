#include "acid_map.hpp"
#include "tree_verifier.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"

TEST(ConsistentMapTest, InvalidateAllDirect) {
    int n = 10000;
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

TEST(ConsistentMapTest, RandomInvalidateDirect) {
    int n = 10000;
    int m = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(m);
    for (int i = 0; i < n; i++) {
        map.emplace(i, i);
    }
    for (int i = 0; i < m; i++) {
        its.push_back(random_element(map));
    }
    its.push_back(map.find(n - 1));
    its.push_back(map.find(n - 2));
    its.push_back(map.begin());
    its.push_back(std::next(map.begin()));
    for (auto it : its) {
        map.erase(it);
        auto it2 = it++;
        EXPECT_FALSE(map.contains(it2->first));
        if (it == map.end()) {
            continue;
        }
        EXPECT_TRUE(map.contains(it->first));
    }
}

TEST(ConsistentMapTest, RandomInvalidateReverse) {
    int n = 10000;
    int m = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(m + 2);
    for (int i = 0; i < n; i++) {
        map.emplace(i, i);
    }
    for (int i = 0; i < m; i++) {
        auto it = random_element(map);
        its.push_back(it);
    }
    its.push_back(map.find(n - 1));
    its.push_back(map.find(n - 2));
    its.push_back(map.begin());
    its.push_back(std::next(map.begin()));
    for (auto it : its) {
        map.erase(it);
        auto it2 = it--;
        EXPECT_FALSE(map.contains(it2->first));
        if (it == map.end()) {
            continue;
        }
        EXPECT_TRUE(map.contains(it->first));
    }
}

TEST(ConsistentMapTest, RandomStepsInvalidate) {
    int n = 10000;
    int m = 1000;
    int p = 1000;
    polyndrom::acid_map<int, int> map;
    std::vector<decltype(map.begin())> its;
    its.reserve(m);
    for (int i = 0; i < n; i++) {
        map.emplace(i, i);
    }
    for (int i = 0; i < m; i++) {
        its.push_back(random_element(map));
    }
    its.push_back(map.find(n - 1));
    its.push_back(map.find(n - 2));
    its.push_back(map.begin());
    its.push_back(std::next(map.begin()));
    int_generator dir_generator(0, 1);
    int_generator step_generator(0, p);
    for (auto it : its) {
        int dir = dir_generator.next_value();
        int step = step_generator.next_value();
        for (int i = 0; i < step; i++) {
            if (dir == 0) {
                if (it->first == n - 1) {
                    break;
                }
                ++it;
            } else {
                if (it->first == 0) {
                    break;
                }
                --it;
            }
        }
    }
    for (auto it : its) {
        map.erase(it);
        EXPECT_FALSE(map.contains(it->first));
        auto it2 = it++;
        if (it == map.end()) {
            continue;
        }
        EXPECT_TRUE(map.contains(it->first));
    }
}