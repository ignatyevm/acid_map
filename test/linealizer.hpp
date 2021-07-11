#pragma once

#include "acid_map.hpp"
#include "utils.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <functional>

using Map = polyndrom::acid_map<int, int>;
using Values = std::vector<int>;
using ValuesIt = typename Values::iterator;
using ActionFunc = std::function<void(Map &, ValuesIt, ValuesIt)>;
using ValuesTransformFunc = std::function<Values(const Values &)>;

static ValuesTransformFunc random_shuffle_func = [](const Values &base_values) -> Values {
    Values values(base_values);
    std::random_shuffle(values.begin(), values.end(), [](int n) {
        return random_int(0, n - 1);
    });
    return values;
};

class Range {
public:
    Range() = default;

    Range(int start, int end) : start(start), end(end) {}

    inline int Start() {
        return start;
    }

    inline int End() {
        return end;
    }

    inline int Length() {
        return End() - Start();
    }

private:
    int start = 0;
    int end = 0;
};

static std::unordered_map<std::string_view, ActionFunc> actions;

class Action {
public:
    static void Register(std::string_view name, ActionFunc func) {
        actions.emplace(name, func);
    }
    static ActionFunc Get(std::string_view action_name) {
        return actions.at(action_name);
    }
};

class ActionRegister {
public:
    ActionRegister(std::string_view name, ActionFunc func) {
        Action::Register(name, func);
    }
};

#define RegisterAction(name) RegisterAction2(name, __LINE__)
#define RegisterAction2(name, line)  RegisterAction3(name, line)
#define RegisterAction3(name, line) \
void action_func_##line(Map &map, ValuesIt first_it, ValuesIt last_it); \
ActionRegister action_register##line(name, action_func_##line); \
void action_func_##line(Map &map, ValuesIt first_it, ValuesIt last_it)

class TestTask {
public:
    TestTask(std::string_view action_name, int threads_count, Range range,
             ValuesTransformFunc transform_func = random_shuffle_func)
            : threads_count(threads_count), action_func(Action::Get(action_name)) {
        values.reserve(threads_count);
        Values base_values(range.Length());
        std::iota(base_values.begin(), base_values.end(), range.Start());
        for (int i = 0; i < threads_count; i++) {
            values.push_back(transform_func(base_values));
        }
    }

    int threads_count;
    ActionFunc action_func;
    std::vector<Values> values;
};

class Linealizer {
public:
    void FillDefault(int n) {
        Values default_values;
        default_values.resize(n);
        std::iota(default_values.begin(), default_values.end(), 0);
        for (int v : default_values) {
            map.emplace(v, 100 * v);
            checker.emplace(v, 100 * v);
        }
    }
    void RegisterTask(std::string_view action_name, int thread_count, Range val_range,
                      ValuesTransformFunc transform_func = random_shuffle_func) {
        threads_count += thread_count;
        tasks.emplace_back(action_name, thread_count, val_range, transform_func);
    }
    void RunTasks() {
        threads.reserve(threads_count);
        for (TestTask &task : tasks) {
            for (int i = 0; i < task.threads_count; i++) {
                threads.emplace_back(task.action_func, std::ref(map), task.values[i].begin(), task.values[i].end());
            }
        }
        for (std::thread &thread : threads) {
            thread.join();
        }
    }
    void VerifyResult() {
        for (TestTask &task : tasks) {
            for (int i = 0; i < task.threads_count; i++) {
                task.action_func(checker, task.values[i].begin(), task.values[i].end());
            }
        }
        EXPECT_EQ(map.size(), checker.size());
        auto it1 = map.begin();
        auto it2 = checker.begin();
        while (it2 != checker.end()) {
            EXPECT_EQ(*it1, *it2);
            ++it1;
            ++it2;
        }
    }
private:
    Map map;
    Map checker;
    std::vector<TestTask> tasks;
    int threads_count = 0;
    std::vector<std::thread> threads;
};