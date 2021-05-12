#include "gtest/gtest.h"

#include "acid_map.hpp"
#include "pure_map_tester.h"
#include "tree_verifier.hpp"

TEST(PureMap, EmptyMap) {
    polyndrom::acid_map<int, int> map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());
    EXPECT_TRUE(verify_tree(map));
}


