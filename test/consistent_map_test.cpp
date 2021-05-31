#include "acid_map.hpp"
#include "tree_verifier.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"

TEST(ConsistentMap, Erase) {
    polyndrom::acid_map<int, int> m;
    for (int i = 0; i < 10; i++) {
        m.emplace(i, 1);
    }
    auto it1 = m.find(1);
    auto it2 = m.find(2);
    auto it3 = m.find(3);
    auto it4 = it1;
    m.erase(it1);
    m.erase(it2);
    m.erase(it3);
    ++it4;
    std::cout << it4->first << std::endl;
    EXPECT_EQ(it4->first, 4);
}