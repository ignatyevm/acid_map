#include "acid_map.hpp"
#include "default_map_tester.h"
#include "tree_verifier.hpp"

#include <vector>
#include <random>

#include "gtest/gtest.h"

std::vector<int> generate_values(int n, int start, int end) {
    std::vector<int> values;
    values.reserve(n);
    std::random_device device;
    std::uniform_int_distribution<int> distribution(start, end);
    for (int i = 0; i < n; i++) {
        values.push_back(distribution(device));
    }
    return values;
}

TEST(DefaultMap, test1) {
    polyndrom::default_map_tester<int, int> default_map_tester;
    std::vector<int> values = generate_values(100000, 0, 1000000);
    for (int v : values) {
        default_map_tester.insert(std::make_pair(v, v));
    }
    /*int* a = new int(5);
    a = new int(10);*/
    EXPECT_TRUE(polyndrom::verify_tree(default_map_tester.testable_map()));
}


