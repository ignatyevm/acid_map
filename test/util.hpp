#include <random>
#include <string>
#include <algorithm>

class complex_object {
public:
    complex_object() : num_(0), str_() {}
    complex_object(const complex_object& other) : num_(other.num_), str_(other.str_), has_copied_(true) {}
    complex_object& operator=(const complex_object& other) {
        num_ = other.num_;
        str_ = other.str_;
        return *this;
    }
    complex_object(complex_object&& other) noexcept : num_(other.num_), str_(std::move(other.str_)), has_moved_(true) {}
    complex_object& operator=(complex_object&& other) noexcept {
        num_ = other.num_;
        str_ = std::move(other.str_);
        return *this;
    }
    complex_object(int num, std::string&& str) : num_(num), str_(std::move(str)), has_emplaced_(true) {}
    complex_object(int num, const std::string& str) : num_(num), str_(str), has_emplaced_(true) {}
    bool operator<(const complex_object& rhs) const {
        return std::make_tuple(num_, str_) < std::make_tuple(rhs.num_, rhs.str_);
    }
    bool operator==(const complex_object& rhs) const {
        return std::make_tuple(num_, str_) == std::make_tuple(rhs.num_, rhs.str_);
    }
    bool has_copied() const {
        return has_copied_ && !has_moved_ && !has_emplaced_;
    }
    bool has_moved() const {
        return !has_copied_ && has_moved_ && !has_emplaced_;
    }
    bool has_emplaced() const {
        return !has_copied_ && !has_moved_ && has_emplaced_;
    }
    bool is_flags_clear() const {
        return !has_copied_ && !has_moved_ && !has_emplaced_;
    }
    void clear_flags() {
        has_copied_ = false;
        has_moved_ = false;
        has_emplaced_ = false;
    }
    int num_;
    std::string str_;
    bool has_copied_ = false;
    bool has_moved_ = false;
    bool has_emplaced_ = false;
};

std::ostream& operator<<(std::ostream& os, const complex_object& key) {
    os << key.num_ << " " << key.str_;
    return os;
}

class int_generator {
public:
    int_generator(int start, int end) : distribution_(start, end) {}
    int next_value() {
        return distribution_(device_);
    }
private:
    std::random_device device_;
    std::uniform_int_distribution<int> distribution_;
};

class string_generator {
public:
    string_generator(int min_len, int max_len) : len_distribution_(min_len, max_len), char_distribution_('a', 'z') {}
    std::string next_value() {
        int len = len_distribution_(device_);
        std::string result(len, ' ');
        for (char& ch : result) {
            ch = char_distribution_(device_);
        }
        return result;
    }
private:
    std::random_device device_;
    std::uniform_int_distribution<int> len_distribution_;
    std::uniform_int_distribution<int> char_distribution_;
};

class complex_object_generator {
public:
    complex_object_generator() : int_generator_(0, 1), string_generator_(100, 100) {}
    complex_object next_value() {
        complex_object object(int_generator_.next_value(), string_generator_.next_value());
        object.clear_flags();
        return object;
    }
    std::string random_string() {
        return string_generator_.next_value();
    }
private:
    int_generator int_generator_;
    string_generator string_generator_;
};

std::vector<int> generate_random_ints(int n, int start, int end) {
    std::vector<int> values;
    values.reserve(n);
    int_generator generator(start, end);
    for (int i = 0; i < n; i++) {
        values.push_back(generator.next_value());
    }
    return values;
}

std::vector<int> generate_asc_sorted_ints(int n, int start, int end) {
    std::vector<int> values = generate_random_ints(n, start, end);
    std::sort(values.begin(), values.end());
    return values;
}

std::vector<int> generate_desc_sorted_ints(int n, int start, int end) {
    std::vector<int> values = generate_asc_sorted_ints(n, start, end);
    std::reverse(values.begin(), values.end());
    return values;
}

std::vector<std::string> generate_random_strings(int n, int min_len, int max_len) {
    std::vector<std::string> values;
    values.reserve(n);
    string_generator generator(min_len, max_len);
    for (int i = 0; i < n; i++) {
        values.push_back(generator.next_value());
    }
    return values;
}

template <class C>
typename C::iterator random_element(C& c) {
    int_generator generator(0, (int) c.size() - 3);
    auto it = c.begin();
    std::advance(it, generator.next_value());
    return it;
}

complex_object make_unique_object(complex_object_generator& objects_generator) {
    complex_object object = objects_generator.next_value();
    object.num_ = 2;
    return object;
}
