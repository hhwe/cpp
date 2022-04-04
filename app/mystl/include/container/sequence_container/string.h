#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

#include <cstring>
#include <iostream>

namespace MyStl {
class String {
public:
    String(const char* cstr = 0) {
        if (cstr) {
            data_ = new char[strlen(cstr) + 1];
            strcpy(data_, cstr);
        } else {
            data_ = new char[1];
            *data_ = '\0';
        }
    }
    String(const String& str) {
        data_ = new char[strlen(str.data_) + 1];
        strcpy(data_, str.data_);
    }
    String& operator=(const String& str) {
        if (this == &str) { return *this; }
        delete[] data_;
        data_ = new char[strlen(str.data_) + 1];
        strcpy(data_, str.data_);
        return *this;
    }
    ~String() {
        delete[] data_;
    }

    char* get_c_str() const {
        return data_;
    }

private:
    char* data_;
};

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.get_c_str();
}
} // namespace MyStl

#endif // MYSTL_STRING_H_
