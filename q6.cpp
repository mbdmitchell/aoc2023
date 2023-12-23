#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split
#include <fstream>
#include <fmt/format.h>
#include <iostream>
#include <numeric>

using Time = int;
using Distance = int;

template<typename T>
[[nodiscard]] std::vector<T> vectorize(std::istream& is) {
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

std::vector<std::pair<Time, Distance>> parse(std::ifstream& data) {
    /// NB: Guaranteed that input data correctly formatted
    auto parsed = tokenize(vectorize<std::string>(data));

    std::vector<std::pair<Time, Distance>> result;

    for (size_t i = 1; i < parsed[0].size(); ++i) {
        int time = std::stoi(parsed[0][i]);
        int distance = std::stoi(parsed[1][i]);
        result.emplace_back(time, distance);
    }

    return result;
}

std::pair<long, long> calc_poss_times(long race_time, long total_distance) {
    const long a = 1, b = -race_time, c = total_distance;
    const double sqrt_determinant = std::sqrt((b * b) - (4 * a * c));
    double bigger = (static_cast<double>(-b) + sqrt_determinant) / (2 * a);
    double littler = (static_cast<double>(-b) - sqrt_determinant) / (2 * a);
    if (littler > bigger){
        std::swap(littler, bigger);
    }
    return {littler, bigger};
    // distance = (race_time - button_press_time) * button_press_time;
    // => button_press_time^2 - (race_time * button_press_time) + total_distance == 0
}

int number_of_ways_to_beat_record(const std::pair<double, double>& range) {
    const auto low = (std::ceil(range.first) == range.first) ? std::ceil(range.first + 1) : std::ceil(range.first);
    const auto high = (std::floor(range.second) == range.second) ? std::floor(range.second - 1) : std::floor(range.second);
    const auto total_ways = (high - low) + 1;
    return static_cast<int>(total_ways);
}

std::pair<long, long> part_2_kerning_adjustment(const std::vector<std::pair<Time, Distance>>& time_dist_pairs) {
    std::string time{};
    std::string distance{};
    for (const auto& p : time_dist_pairs) {
        time += std::to_string(static_cast<int>(p.first));
        distance += std::to_string(static_cast<int>(p.second));
    }
    return {std::stol(time), std::stol(distance)};
}

int main() {

    std::ifstream data("../times.txt");
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    const std::vector<std::pair<Time, Distance>> time_dist_pairs = parse(data);

    const int part_1 = std::reduce(cbegin(time_dist_pairs), cend(time_dist_pairs), 1, [](int acc, const auto& pair) {
        return acc * number_of_ways_to_beat_record(calc_poss_times(pair.first, pair.second));
    });

    const int part_2 = [&](){
        const auto pair = part_2_kerning_adjustment(time_dist_pairs);
        return number_of_ways_to_beat_record(calc_poss_times(pair.first, pair.second));
    }();

    fmt::print("Part 1: {}\n", part_1);
    fmt::print("Part 2: {}\n", part_2);

    return 0;
}
