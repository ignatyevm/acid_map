#include "gtest/gtest.h"
#include "acid_map.hpp"

TEST(PureMap, EmptyMap) {
    polyndrom::acid_map<int, int> map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());
    EXPECT_TRUE(polyndrom::verify_tree(map));
}


