#include "Utils.h"

std::vector<char> vectorize_each_char(std::string_view str) {
    std::vector<char> vec;
    for (const auto c : str) {
        vec.push_back(c);
    }
    return vec;
}