#pragma once

#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split

template<typename T>
[[nodiscard]] std::vector<T> vectorize(std::ifstream& is) {
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

[[nodiscard]] std::vector<Hand_And_Bid> parse(std::ifstream& data, const Part* part) {
    /// NB: Guaranteed that input data correctly formatted
    auto parsed = tokenize(vectorize<std::string>(data));

    std::vector<Hand_And_Bid> result;

    for (auto& row : parsed) {
        result.emplace_back(Hand_And_Bid{Hand{row[0], part} , std::stoi(row[1])});
    }

    return result;
}
