#include "utils.hpp"

complex_object::complex_object() : num_(0), str_() {}

complex_object::complex_object(const complex_object& other) : num_(other.num_), str_(other.str_), has_copied_(true) {}

complex_object& complex_object::operator=(const complex_object& other) {
    num_ = other.num_;
    str_ = other.str_;
    return *this;
}

complex_object::complex_object(complex_object&& other) noexcept
    : num_(other.num_), str_(std::move(other.str_)), has_moved_(true) {}

complex_object& complex_object::operator=(complex_object&& other) noexcept {
    num_ = other.num_;
    str_ = std::move(other.str_);
    return *this;
}

complex_object::complex_object(int num, std::string&& str) : num_(num), str_(std::move(str)), has_emplaced_(true) {}

complex_object::complex_object(int num, const std::string& str) : num_(num), str_(str), has_emplaced_(true) {}

bool complex_object::operator<(const complex_object& rhs) const {
    return std::make_tuple(num_, str_) < std::make_tuple(rhs.num_, rhs.str_);
}

bool complex_object::operator==(const complex_object& rhs) const {
    return std::make_tuple(num_, str_) == std::make_tuple(rhs.num_, rhs.str_);
}

bool complex_object::has_copied() const {
    return has_copied_ && !has_moved_ && !has_emplaced_;
}

bool complex_object::has_moved() const {
    return !has_copied_ && has_moved_ && !has_emplaced_;
}

bool complex_object::has_emplaced() const {
    return !has_copied_ && !has_moved_ && has_emplaced_;
}

bool complex_object::is_flags_clear() const {
    return !has_copied_ && !has_moved_ && !has_emplaced_;
}

void complex_object::clear_flags() {
    has_copied_ = false;
    has_moved_ = false;
    has_emplaced_ = false;
}

std::ostream& operator<<(std::ostream& os, const complex_object& key) {
    os << key.num_ << " " << key.str_;
    return os;
}

int_generator::int_generator(int start, int end) : distribution_(start, end) {}

int int_generator::next_value() {
    return distribution_(device_);
}

string_generator::string_generator(int min_len, int max_len) : len_distribution_(min_len, max_len), char_distribution_('a', 'z') {}

std::string string_generator::next_value() {
    int len = len_distribution_(device_);
    std::string result(len, ' ');
    for (char& ch : result) {
        ch = char_distribution_(device_);
    }
    return result;
}

complex_object_generator::complex_object_generator() : int_generator_(0, 1), string_generator_(100, 100) {}

complex_object complex_object_generator::next_value() {
    complex_object object(int_generator_.next_value(), string_generator_.next_value());
    object.clear_flags();
    return object;
}

complex_object make_unique_object(complex_object_generator& objects_generator) {
    complex_object object = objects_generator.next_value();
    object.num_ = 2;
    return object;
}