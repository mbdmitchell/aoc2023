#include "Utils.h"

UniverseMap tokenize(const std::vector<std::string> &lines_of_data) {

    UniverseMap tokenized_lines_of_data;
    for (const auto& line : lines_of_data) {
        const std::vector<char> tokens = [&](){
            std::vector<char> tokens;
            for (const char c : line){
                tokens.push_back(c);
            }
            return tokens;
        }();
        tokenized_lines_of_data.push_back(tokens);
    }

    return tokenized_lines_of_data;
}
