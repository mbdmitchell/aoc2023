#pragma once

#include <fstream>
#include <iostream>
#include <vector>

using UniverseMap = std::vector<std::vector<char>>;
using MultiplierMap = std::vector<std::vector<int>>;

[[nodiscard]] UniverseMap tokenize(const std::vector<std::string>& lines_of_data);

template<typename T>
[[nodiscard]] std::vector<T> vectorize(std::ifstream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

