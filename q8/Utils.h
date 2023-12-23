#pragma once

#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split
#include <fstream>
#include <string>

template<typename T>
std::vector<T> vectorize(std::istream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

template<typename T>
[[nodiscard]] std::vector<T> tokenize(const T& lines_of_data) {
    std::vector<T> tokenized_lines_of_data;
    for (const auto& line : lines_of_data) {
        T tokens;
        boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
        tokenized_lines_of_data.push_back(tokens);
    }
    return tokenized_lines_of_data;
}

std::vector<char> vectorize_each_char(std::string_view str);
