#include <iostream>
#include <numeric>
#include <fstream>
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
[[nodiscard]] std::vector<std::vector<int>> tokenize(const std::vector<T>& lines_of_data) {
    std::vector<std::vector<int>> tokenized_lines_of_data;

    for (const auto& line : lines_of_data) {

        const std::vector<std::string> tokens = [&](){
            std::vector<std::string> tokens;
            boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
            return tokens;
        }();

        const std::vector<int> int_tokens = [&](){
            std::vector<int> int_tokens;
            int_tokens.reserve(tokens.size());
            for (const auto &token: tokens) {
                int_tokens.push_back(std::stoi(token));
            }
            return int_tokens;
        }();

        tokenized_lines_of_data.push_back(int_tokens);
    }
    return tokenized_lines_of_data;
}

std::vector<std::vector<int>> parse(std::string_view file_name) {
    std::ifstream data(file_name);
    return tokenize(vectorize<std::string>(data));
}

enum class End {FRONT, BACK};

[[nodiscard]] int predict_next_num_in_sequence(std::vector<int> vec, End predicting) {
    if (std::all_of(cbegin(vec), cend(vec), [&](const int num){ return vec[0] == num; })) {
        return vec[0];
    }
    else {
        const auto prev = vec;
        std::adjacent_difference(begin(vec), end(vec), begin(vec));
        vec.erase(begin(vec));
        const auto nth_most = (predicting == End::FRONT) ? prev.front() : prev.back();
        const int multiplier = (predicting == End::FRONT) ? -1 : 1;
        return nth_most + multiplier * predict_next_num_in_sequence(vec, predicting);
    }
}

int main() {
    const auto sequences = parse("../sequences.txt");

    const auto part_1_solution = std::accumulate(cbegin(sequences), cend(sequences), 0, [](int acc, const auto& seq){
        return acc + predict_next_num_in_sequence(seq, End::BACK);
    });
    const auto part_2_solution = std::accumulate(cbegin(sequences), cend(sequences), 0, [](int acc, const auto& seq){
        return acc + predict_next_num_in_sequence(seq, End::FRONT);
    });

    std::cout << part_1_solution << ' ' << part_2_solution;
    return 0;
}
