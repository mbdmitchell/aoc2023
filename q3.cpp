#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>

using Iter = std::string::const_iterator;

struct NeighbourInfo {
    bool has_neighbour;
    std::set<std::pair<size_t, size_t>> gear_locations{};
};

struct NumData {
    int num;
    bool is_valid;
    NeighbourInfo neighbour_info;
};

bool is_symbol(char c){
    return !isdigit(c) && c != '.';
}

std::pair<int, int> clamp_ixs(std::pair<size_t, size_t> row_col, std::pair<size_t, size_t> row_col_sizes) {
    return {std::clamp(static_cast<int>(row_col.first), 0, static_cast<int>(row_col_sizes.first-1))
    , std::clamp(static_cast<int>(row_col.second), 0, static_cast<int>(row_col_sizes.second-1))};
}

NeighbourInfo get_neighbour_info(const std::vector<std::string>& lines, size_t row, size_t col) {

    NeighbourInfo ni {.has_neighbour = false};

    for (int col_delta = -1; col_delta <= 1; ++col_delta) {
        for (int row_delta = -1; row_delta <= 1; ++row_delta) {

            const auto& [clamped_row, clamped_col]
                = clamp_ixs({row+row_delta, col+col_delta}, {lines.size(), lines[0].size()});

            if (clamped_row - row == 0 && clamped_col - col == 0) {
                continue;
            }

            if (is_symbol(lines[clamped_row][clamped_col])) {
                ni.has_neighbour = true;
                if (lines[clamped_row][clamped_col] == '*') {
                    ni.gear_locations.insert({clamped_row, clamped_col});
                }
            }

        }
    }

    return ni;
}

Iter pos_of_first_digit(Iter pos, const std::string& line) {
    /// return iter to first digit from `pos` (inclusive)
    if (pos == cend(line)) return pos;
    return std::find_if(pos, cend(line), [](const char c) {
        return std::isdigit(c);
    });
}

Iter pos_of_first_non_digit(Iter pos, const std::string& line) {
    /// return iter to first non-digit from `pos` (inclusive)
    if (pos == cend(line)) return pos;
    return std::find_if_not(pos, cend(line), [](const char c){
        return std::isdigit(c);
    });
}

int num_from_iter_range(Iter start, Iter finish){
    ///      . . 4 3 2 5 % . .
    /// it:      ^       ^
    int num = 0;
    for (auto it = start; it < finish; ++it) {
        num = num * 10 + (*it - '0');
    }
    return num;
}

NumData build_numdata_entry(Iter pos, Iter end_pos, const std::vector<std::string>& lines, size_t r_ix) {

    const auto& current_row = lines[r_ix];

    bool is_valid = [&](){
        for (auto p = pos; p < end_pos; ++p) {
            const size_t c_ix = std::distance(cbegin(current_row), p);
            const NeighbourInfo info = get_neighbour_info(lines, r_ix, c_ix);
            if (info.has_neighbour) {
                return true;
            }
        }
        return false;
    }();

    const std::set<std::pair<size_t, size_t>> gear_locations = [&](){
        std::set<std::pair<size_t, size_t>> gl;
        for (auto p = pos; p < end_pos; ++p) {
            const size_t c_ix = std::distance(cbegin(current_row), p);
            const NeighbourInfo info = get_neighbour_info(lines, r_ix, c_ix);
            gl.insert(cbegin(info.gear_locations), cend(info.gear_locations));
        }
        return gl;
    }();

    const NumData entry {
        .num = num_from_iter_range(pos, end_pos),
        .is_valid = is_valid,
        .neighbour_info = {is_valid, gear_locations}
    };

    return entry;
}

std::vector<NumData> extract_num_data(const std::vector<std::string>& lines) {

    std::vector<NumData> num_data;

    for (size_t r_ix = 0; r_ix < lines.size(); ++r_ix) {

        const std::string& current_row = lines[r_ix];
        std::string::const_iterator pos = cbegin(current_row), end_pos;

        while (pos != cend(current_row)) {

            pos = pos_of_first_digit(pos, current_row);
            end_pos = pos_of_first_non_digit(pos, current_row);

            const NumData entry = build_numdata_entry(pos, end_pos, lines, r_ix);

            if (entry.num != 0) {
                num_data.push_back(entry);
            }

            pos = (end_pos == cend(current_row)) ? cend(current_row) : end_pos + 1;
        }

    }

    return num_data;
}

std::vector<NumData> extract_ordered_num_data(const std::vector<std::string>& data) {
    std::vector<NumData> n = extract_num_data(data);
    std::sort(begin(n), end(n), [&](const NumData& a, const NumData& b){
        return a.neighbour_info.gear_locations < b.neighbour_info.gear_locations;
    });
    return n;
}

template<typename T>
std::vector<T> vectorize(std::ifstream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

int main() {

    std::ifstream data("../data.txt");
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    const std::vector<NumData> nums = [&](){
        const std::vector<std::string> lines = vectorize<std::string>(data);
        return extract_ordered_num_data(lines);
    }();

    const int part_1 = std::accumulate(cbegin(nums), cend(nums), 0, [&](int acc, const NumData& nd) {
        const int val = nd.is_valid ? nd.num : 0;
        return acc + val;
    });

    const int part_2 = [&](){
        int running_total = 0;

        const auto start = std::find_if_not(cbegin(nums), cend(nums), [&](const auto& n){
            return n.neighbour_info.gear_locations.empty();
        });

        for (auto it = start; it < cend(nums) - 1; ++it) {
            if (it->neighbour_info.gear_locations == (it+1)->neighbour_info.gear_locations) {
                running_total += (it->num * (it+1)->num);
            }
        }
        return running_total;
    }();

    fmt::print("Part 1: {}\n", part_1);
    fmt::print("Part 2: {}\n", part_2);

    return 0;
}
