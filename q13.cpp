#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <numeric>

using AshRockMap = std::vector<std::vector<char>>;

// ORGANIZE DATA

std::vector<AshRockMap> split(const AshRockMap& map) {
    std::vector<AshRockMap> split_input;
    AshRockMap current;
    for (const auto& line : map) {
        if (!line.empty()) {
            current.push_back(line);
        }
        else {
            split_input.push_back(current);
            current.clear();
        }
    }
    split_input.push_back(current);
    return split_input;
}

AshRockMap tokenize(const std::vector<std::string> &lines_of_data) {
    AshRockMap tokenized_lines_of_data;
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

template<typename T>
[[nodiscard]] std::vector<T> vectorize(std::ifstream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

std::vector<AshRockMap> parse(std::string_view file_name) {
    std::ifstream data(file_name);
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    std::vector<AshRockMap> map = split(tokenize(vectorize<std::string>(data)));
    return map;
}

// HELPERS

bool one_off_row_match(size_t row_ix_a, size_t row_ix_b, const AshRockMap& map) {
    const auto& row_a = map[row_ix_a];
    const auto& row_b = map[row_ix_b];

    unsigned mismatchCount = std::inner_product(begin(row_a), end(row_a), begin(row_b), 0, std::plus<>(), [](char a, char b) {
        return (a != b) ? 1 : 0;
    });

    return mismatchCount == 1;
}
bool one_off_col_match(size_t col_ix_a, size_t col_ix_b, const AshRockMap& map) {
    return std::ranges::count_if(map, [&](const auto& row) {
        return row[col_ix_a] != row[col_ix_b];
    }) == 1;
}

bool compare_verticals(size_t ix_a, size_t ix_b, const AshRockMap& map) {
    for (size_t r = 0; r < map.size(); ++r) {
        if (map[r][ix_a] != map[r][ix_b]) {
            return false;
        }
    }
    return true;
};

bool is_horizontal_reflection(size_t starting_ix, const AshRockMap& map, unsigned smudges_remaining = 0) {
    size_t upper = starting_ix-1;
    size_t lower = starting_ix+2;
    while (upper != std::numeric_limits<size_t>::max() && lower < map.size()) { // upper: ... -> 1 -> 0 -> num_limit::max()
        if (map[upper] != map[lower]) {
            if (smudges_remaining == 0 || !one_off_row_match(upper, lower, map)) {
                return false;
            }
            --smudges_remaining;
        }
        --upper;
        ++lower;
    }
    return smudges_remaining == 0;
}
bool is_vertical_reflection(size_t starting_ix, const AshRockMap& map, unsigned smudges_remaining = 0) {
    size_t left = starting_ix-1;
    size_t right = starting_ix+2;
    while (left != std::numeric_limits<size_t>::max() && right < map.front().size()) { // left: ... -> 1 -> 0 -> num_limit::max()
        if (!compare_verticals(left, right, map)) {
            if (smudges_remaining == 0 || !one_off_col_match(left, right, map)) {
                return false;
            }
            --smudges_remaining;
        }
        --left;
        ++right;
    }
    return smudges_remaining == 0;
};

std::vector<size_t> get_upper_ix_of_all_near_horizontal_twins(const AshRockMap& map) {
    std::vector<size_t> upper_ixes;
    for (size_t r = 0; r < map.size()-1; ++r) {
        if (one_off_row_match(r, r+1, map)) {
            upper_ixes.push_back(r);
        }
    }
    return upper_ixes;
}
std::vector<size_t> get_left_ix_of_all_near_vertical_twins(const AshRockMap& map) {
    std::vector<size_t> left_ixes;
    for (size_t col = 0; col < map.front().size()-1; ++col) {
        if (one_off_col_match(col, col+1, map)) {
            left_ixes.push_back(col);
        }
    }
    return left_ixes;
}
std::vector<size_t> get_upper_ix_of_horizontal_reflections(const std::vector<size_t>& candidates, const AshRockMap& map, unsigned smudges_remaining = 0) {
    std::vector<size_t> successful;
    for (const auto& c : candidates) {
        if (is_horizontal_reflection(c, map, smudges_remaining)) {
            successful.push_back(c);
        }
    }
    return successful;
}
std::vector<size_t> get_left_ix_of_vertical_reflections(const std::vector<size_t>& candidates, const AshRockMap& map, unsigned smudges_remaining = 0) {
    std::vector<size_t> successful;
    for (const auto& c : candidates) {
        if (is_vertical_reflection(c, map, smudges_remaining)) {
            successful.push_back(c);
        }
    }
    return successful;
}
std::vector<size_t> get_upper_ix_of_all_horizontal_twins(const AshRockMap& map) {
    std::vector<size_t> upper_ixes;
    for (size_t i = 0; i < map.size()-1; ++i) {
        if (map[i] == map[i+1]) {
            upper_ixes.push_back(i);
        }
    }
    return upper_ixes;
}
std::vector<size_t> get_left_ix_of_all_vertical_twins(const AshRockMap& map) {
    std::vector<size_t> left_ixes;
    for (size_t col = 0; col < map.front().size()-1; ++col) {
        if (compare_verticals(col, col+1, map)) {
            left_ixes.push_back(col);
        }
    }
    return left_ixes;
}

// REFLECTION_INFO

struct ReflectionInfo {
    std::vector<size_t> left_ix_of_vertical_reflection;
    std::vector<size_t> upper_ix_of_horizontal_reflection;
};

ReflectionInfo calc_ReflectionInfo(const AshRockMap& map) {
    const auto vertical_candidates = get_left_ix_of_all_vertical_twins(map);
    const auto horizontal_candidates = get_upper_ix_of_all_horizontal_twins(map);
    return {
        .left_ix_of_vertical_reflection = get_left_ix_of_vertical_reflections(vertical_candidates, map),
        .upper_ix_of_horizontal_reflection = get_upper_ix_of_horizontal_reflections(horizontal_candidates, map)
    };
}
ReflectionInfo calc_smudged_ReflectionInfo(const AshRockMap& map) {
    const auto near_col_twins = get_left_ix_of_all_near_vertical_twins(map);
    const auto near_row_twins = get_upper_ix_of_all_near_horizontal_twins(map);
    const auto col_twins = get_left_ix_of_all_vertical_twins(map);
    const auto row_twins = get_upper_ix_of_all_horizontal_twins(map);

    auto append_reflections = [](std::vector<size_t>& base, const std::vector<size_t>& reflections) {
        base.insert(base.end(), reflections.begin(), reflections.end());
    };

    std::vector<size_t> smudgey_rows;
    append_reflections(smudgey_rows, get_upper_ix_of_horizontal_reflections(near_row_twins, map, 0));
    append_reflections(smudgey_rows, get_upper_ix_of_horizontal_reflections(row_twins, map, 1));

    std::vector<size_t> smudgey_cols;
    append_reflections(smudgey_cols, get_left_ix_of_vertical_reflections(near_col_twins, map, 0));
    append_reflections(smudgey_cols, get_left_ix_of_vertical_reflections(col_twins, map, 1));

    return {
        .left_ix_of_vertical_reflection = std::move(smudgey_cols),
        .upper_ix_of_horizontal_reflection = std::move(smudgey_rows)
    };
}


// SUMMARIZE

unsigned summarize(const ReflectionInfo& info) {
    const auto& vertical_reflections = info.left_ix_of_vertical_reflection;
    const unsigned verticalSum = std::accumulate(
            begin(vertical_reflections), end(vertical_reflections), 0,
            [](unsigned acc, const auto& col_ix) { return acc + col_ix + 1; }
    );

    const auto& horizontal_reflections = info.upper_ix_of_horizontal_reflection;
    const unsigned horizontalSum = std::accumulate(
            begin(horizontal_reflections), end(horizontal_reflections), 0,
            [](unsigned acc, const auto& row_ix) { return acc + (row_ix + 1) * 100; }
    );

    return verticalSum + horizontalSum;
}

unsigned map_to_num(const AshRockMap& map, bool is_smudged = false) {
    const ReflectionInfo info = is_smudged ? calc_smudged_ReflectionInfo(map) : calc_ReflectionInfo(map);
    return summarize(info);
}

// SOLUTIONS

unsigned part_1(const std::vector<AshRockMap>& maps) {
    return std::accumulate(begin(maps), end(maps), 0u, [](unsigned acc, const AshRockMap& map){
        return acc + map_to_num(map, false);
    });
}
unsigned part_2(const std::vector<AshRockMap>& maps) {
    return std::accumulate(begin(maps), end(maps), 0u, [](unsigned acc, const AshRockMap& map){
        return acc + map_to_num(map, true);
    });
}

int main() {

    const std::vector<AshRockMap> input = parse("../maps.txt");
    fmt::print("Part 1: {}\n", part_1(input));
    fmt::print("Part 2: {}\n", part_2(input));
    return 0;
}
