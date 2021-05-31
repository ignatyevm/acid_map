#pragma once

#include <random>
#include <string>
#include <algorithm>
#include <ostream>

class complex_object {
public:
    //friend std::ostream& operator<<(std::ostream& os, const complex_object& key);
    complex_object();
    complex_object(const complex_object& other);
    complex_object& operator=(const complex_object& other);
    complex_object(complex_object&& other) noexcept;
    complex_object& operator=(complex_object&& other) noexcept;
    complex_object(int num, std::string&& str);
    complex_object(int num, const std::string& str);
    bool operator<(const complex_object& rhs) const;
    bool operator==(const complex_object& rhs) const;
    bool has_copied() const;
    bool has_moved() const;
    bool has_emplaced() const;
    bool is_flags_clear() const;
    void clear_flags();
    int num_;
    std::string str_;
    bool has_copied_ = false;
    bool has_moved_ = false;
    bool has_emplaced_ = false;
};

std::ostream& operator<<(std::ostream& os, const complex_object& key);

class int_generator {
public:
    int_generator(int start, int end);
    int next_value();
private:
    std::random_device device_;
    std::uniform_int_distribution<int> distribution_;
};

class string_generator {
public:
    string_generator(int min_len, int max_len);
    std::string next_value();
private:
    std::random_device device_;
    std::uniform_int_distribution<int> len_distribution_;
    std::uniform_int_distribution<int> char_distribution_;
};

class complex_object_generator {
public:
    complex_object_generator();
    complex_object next_value();
private:
    int_generator int_generator_;
    string_generator string_generator_;
};

template <class C>
typename C::iterator random_element(C& c) {
    int_generator generator(0, (int) c.size() - 3);
    auto it = c.begin();
    std::advance(it, generator.next_value());
    return it;
}

complex_object make_unique_object(complex_object_generator& objects_generator);
