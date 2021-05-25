#include <random>
#include <string>
#include <algorithm>

class complex_key {
public:
    complex_key() = default;
    complex_key(const complex_key& other) : a_(other.a_), b_(other.b_), c_(other.c_), has_copied_(true) {}
    complex_key& operator=(const complex_key& other) {
        a_ = other.a_;
        b_ = other.b_;
        c_ = other.c_;
        return *this;
    }
    complex_key(complex_key&& other) noexcept :  a_(other.a_), b_(other.b_), c_(std::move(other.c_)), has_moved_(true) {}
    complex_key& operator=(complex_key&& other) noexcept {
        a_ = other.a_;
        b_ = other.b_;
        c_ = std::move(other.c_);
        return *this;
        return *this;
    }
    complex_key(int a, int b, const std::string& c) : a_(a), b_(b), c_(c), has_emplaced_(true) {}
    bool operator<(const complex_key& rhs) const {
        return std::make_tuple(a_, b_, c_) < std::make_tuple(rhs.a_, rhs.b_, rhs.c_);
    }
    bool operator==(const complex_key& rhs) const {
        return std::make_tuple(a_, b_, c_) == std::make_tuple(rhs.a_, rhs.b_, rhs.c_);
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
    int a_;
    int b_;
    std::string c_;
    bool has_copied_ = false;
    bool has_moved_ = false;
    bool has_emplaced_ = false;
};

std::ostream& operator<<(std::ostream& os, const complex_key& key) {
    os << key.a_ << " " << key.b_ << " " << key.c_;
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

class complex_key_generator {
public:
    complex_key_generator() : int_generator_(0, 1), string_generator_(100, 100) {}
    complex_key next_value() {
        int a = int_generator_.next_value();
        complex_key key(a, a, string_generator_.next_value());
        key.has_emplaced_ = false;
        return key;
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
    int_generator generator(0, (int) c.size() - 2);
    auto it = c.begin();
    std::advance(it, generator.next_value());
    return it;
}
