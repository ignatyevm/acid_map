#include "acid_map.hpp"
#include "linealizer.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <functional>


RegisterAction("find_existing") {
    for (auto val_it = first_it; val_it != last_it; ++val_it) {
        int val = *val_it;
        auto it = map.find(val);
        EXPECT_TRUE(it != map.end());
        EXPECT_EQ(it->first, val);
        EXPECT_EQ(it->second, 100 * val);
    }
}

RegisterAction("find_not_existing") {
    for (auto val_it = first_it; val_it != last_it; ++val_it) {
        int val = *val_it;
        auto it = map.find(val);
        EXPECT_TRUE(it == map.end());
    }
}

RegisterAction("insert") {
    for (auto val_it = first_it; val_it != last_it; ++val_it) {
        int val = *val_it;
        auto[it, flag] = map.insert(std::make_pair(val, 100 * val));
        EXPECT_EQ(it->first, val);
        EXPECT_EQ(it->second, 100 * val);
    }
}

RegisterAction("erase") {
    for (auto val_it = first_it; val_it != last_it; ++val_it) {
        map.erase(*val_it);
    }
}

RegisterAction("erase_by_iterator") {
    for (auto val_it = first_it; val_it != last_it; ++val_it) {
        int val = *val_it;
        auto it = map.find(val);
        if (it != map.end()) {
            map.erase(it);
            EXPECT_EQ(it->first, val);
            EXPECT_EQ(it->second, 100 * val);
        }
    }
}

TEST(AtomicMapTest, ConcurrentInsert) {
    Linealizer linealizer;
    linealizer.RegisterTask("insert", 4, {0, 10000});
    linealizer.RunTasks();
    linealizer.VerifyResult();
}

TEST(AtomicMapTest, ConcurrentErase) {
    Linealizer linealizer;
    linealizer.FillDefault(10000);
    linealizer.RegisterTask("erase", 4, {0, 10000});
    linealizer.RunTasks();
    linealizer.VerifyResult();
}

TEST(AtomicMapTest, ConcurrentEraseByIterator) {
    Linealizer linealizer;
    linealizer.FillDefault(10000);
    linealizer.RegisterTask("erase_by_iterator", 4, {0, 10000});
    linealizer.RunTasks();
    linealizer.VerifyResult();
}

TEST(AtomicMapTest, ConcurrentFind) {
    Linealizer linealizer;
    linealizer.FillDefault(10000);
    linealizer.RegisterTask("find_existing", 2, {0, 10000});
    linealizer.RegisterTask("find_not_existing", 4, {10000, 20000}, [](const Values &values) {
        return values;
    });
    linealizer.RunTasks();
    linealizer.VerifyResult();
}

TEST(AtomicMapTest, ConcurrentFindInsertErase) {
    Linealizer linealizer;
    linealizer.FillDefault(11000);
    linealizer.RegisterTask("insert", 2, {11000, 20000});
    linealizer.RegisterTask("erase", 2, {0, 5000});
    linealizer.RegisterTask("erase_by_iterator", 2, {5000, 10000});
    linealizer.RegisterTask("find_existing", 2, {10000, 11000});
    linealizer.RunTasks();
    linealizer.VerifyResult();
}
