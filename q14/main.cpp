#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <map>
#include <unordered_set>
#include <utility>

#include "Direction.h"
#include "Position.h"
#include "StoneCount.h"

using RockMap = std::vector<std::vector<char>>;
std::ostream& operator<<(std::ostream& os, const RockMap& map) {
    for (const auto& row : map) {
        for (const char col : row) {
            os << col;
        }
        os << '\n';
    }
    os << '\n';
    return os;
}

// PARSE DATA FUNCTIONS

RockMap tokenize(const std::vector<std::string> &lines_of_data) {
    RockMap tokenized_lines_of_data;
    for (const auto& line : lines_of_data) {
        const std::vector<char> tokens = [&](){
            std::vector<char> tokens;
            std::ranges::copy(line, std::back_inserter(tokens));
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

RockMap parse(std::string_view file_name) {
    std::ifstream data(file_name);
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    RockMap map = tokenize(vectorize<std::string>(data));
    return map;
}

// PROBLEM FUNCTIONS

// ... misc

unsigned no_of_rolling_rocks(const Position& position, const RockMap& map, const Direction& direction) {
    /// count no. of rolling rocks from position (normally a stable rock) to next stable rock

    // HELPERS
    auto count_vertical_rolling_rocks = [&](int increment) {
        unsigned vertical_rolling_rocks = 0;
        auto current_row = position.row_ix - direction.row_delta();

        while (0 <= current_row && current_row < map.size() && map[current_row][position.col_ix] != '#') {
            if (map[current_row][position.col_ix] == 'O') {
                ++vertical_rolling_rocks;
            }
            current_row += increment;
        }

        return vertical_rolling_rocks;
    };
    auto count_horizontal_rolling_rocks = [&](int increment) {
        unsigned horizontal_rolling_rocks = 0;
        auto current_col = position.col_ix - direction.col_delta();

        while (0 <= current_col && current_col < map.front().size() && map[position.row_ix][current_col] != '#') {
            if (map[position.row_ix][current_col] == 'O') {
                ++horizontal_rolling_rocks;
            }
            current_col += increment;
        }

        return horizontal_rolling_rocks;
    };

    // -------

    const bool is_vertical_direction = direction == Direction::up() || direction == Direction::down();

    const int increment = [&](){
        if (is_vertical_direction) {
            return (direction == Direction::up()) ? 1 : -1;
        }
        else {
            return (direction == Direction::right()) ? -1 : 1;
        }
    }();

    if (is_vertical_direction) {
        return count_vertical_rolling_rocks(increment);
    }
    else {
        return count_horizontal_rolling_rocks(increment);
    }

}

// ... calc stones

std::vector<Position> calc_edge_stones(const RockMap& map, const Direction& direction) {

    const bool is_vertical_direction = direction == Direction::up() || direction == Direction::down();

    std::vector<Position> positions;

    if (is_vertical_direction) {
        const auto& row_size = map.front().size();
        const gsl::index row_ix = (direction == Direction::up()) ? -1 : gsl::narrow_cast<gsl::index>(map.front().size());
        for (gsl::index col_ix = 0; col_ix < row_size; ++col_ix) {
            positions.emplace_back(row_ix, col_ix);
        }
    }
    else {
        const auto& col_size = map.size();
        const gsl::index col_ix = (direction == Direction::left()) ? -1 : gsl::narrow_cast<gsl::index>(map.size());
        for (gsl::index row_ix = 0; row_ix < col_size; ++row_ix) {
            positions.emplace_back(row_ix, col_ix);
        }
    }

    return positions;
}

std::vector<Position> calc_stable_stone_positions(const RockMap& map) {
    std::vector<Position> pos;
    for (gsl::index row = 0; row < map.size(); ++row) {
        for (gsl::index col = 0; col < map.front().size(); ++col) {
            if (map[row][col] == '#') {
                pos.emplace_back(row, col);
            }
        }
    }
    return pos;
}

std::vector<Position> all_stable_stones(const RockMap& map, const Direction& tilt_direction) {

    std::vector<Position> stable_stones = calc_stable_stone_positions(map);
    std::vector<Position> edge_stones = calc_edge_stones(map, tilt_direction);

    std::vector<Position> stones;
    stones.reserve(stable_stones.size() + edge_stones.size());

    std::ranges::move(stable_stones, std::back_inserter(stones));
    std::ranges::move(edge_stones, std::back_inserter(stones));

    return stones;
}

std::vector<Position> calc_rolling_stones_positions(const std::vector<StoneCount>& counts, Direction direction) {

    std::vector<Position> stone_positions;

    for (const auto& stone_count : counts) {
        Position current_position = stone_count.stone_position;

        for (auto i = 1; i <= stone_count.no_of_stones; ++i) {
            current_position = {current_position.row_ix - direction.row_delta(), current_position.col_ix - direction.col_delta()};
            stone_positions.push_back(current_position);
        }
    }

    return stone_positions;
}

std::vector<Position> calc_rolling_stones_positions(const RockMap& map) {
    std::vector<Position> pos;
    for (gsl::index row = 0; row < map.size(); ++row) {
        for (gsl::index col = 0; col < map.front().size(); ++col) {
            if (map[row][col] == 'O') {
                pos.emplace_back(row, col);
            }
        }
    }
    return pos;
}

// ... calc counts

std::vector<StoneCount> calc_counts_from_n_wards_tilt(const RockMap& map, const Direction& direction) {

    const std::vector<Position> stable_stones = all_stable_stones(map, direction);

    std::vector<StoneCount> counts = [&](){
        std::vector<StoneCount> c;
        for (Position pos : stable_stones) {
            const unsigned rolling_rocks_count = no_of_rolling_rocks(pos, map, direction);
            if (rolling_rocks_count != 0) {
                c.emplace_back(pos, rolling_rocks_count);
            }
        }
        return c;
    }();

    return counts;
}

// tilting functions

void tilter(const std::vector<Position>& stable_stones,
            const std::vector<Position>& rolling_stones,
            RockMap& map) {

    const auto rolling_stones_set = std::unordered_set<Position, PositionHash>(rolling_stones.begin(), rolling_stones.end());
    const auto stable_stones_set = std::unordered_set<Position, PositionHash>(stable_stones.begin(), stable_stones.end());

    //#pragma omp parallel for -- didn't make noticeable difference
    for (gsl::index row = 0; row < map.size(); ++row) {
        for (gsl::index col = 0; col < map.front().size(); ++col) {
            const Position current_position = {row, col};
            if (rolling_stones_set.find(current_position) != rolling_stones_set.end()) {
                map[row][col] = 'O';
            }
            else if (stable_stones_set.find(current_position) != stable_stones_set.end()) {
                map[row][col] = '#';
            }
            else {
                map[row][col] = '.';
            }
        }
    }
}

void tilt(RockMap& map, Direction direction) {

    const std::vector<StoneCount> stable_stones = calc_counts_from_n_wards_tilt(map, direction);
    const std::vector<Position> rolling_stones = calc_rolling_stones_positions(stable_stones, direction);

    tilter(calc_stable_stone_positions(map), rolling_stones, map);
}

// cycle functions

void cycle(RockMap& map){
    tilt(map, Direction::up());
    tilt(map, Direction::left());
    tilt(map, Direction::down());
    tilt(map, Direction::right());
}

gsl::index nth_cycle_equivalent_to(std::pair<gsl::index, gsl::index> cycle_start_and_end, unsigned no_of_cycles) {
    const auto& start = cycle_start_and_end.first;
    const auto& end = cycle_start_and_end.second;
    const auto cycle_size = (end - start) + 1;

    return start + (no_of_cycles - start) % cycle_size;
}

RockMap map_after_n_cycles(RockMap map, unsigned no_of_cycles) {

    std::map<RockMap, gsl::index> cache;
    cache[map] = 0;

    auto find_cycle = [&](RockMap& map) -> std::pair<gsl::index, gsl::index> {
        for (gsl::index current_cycle = 1; current_cycle <= no_of_cycles; ++current_cycle) {
            cycle(map);
            if (cache.contains(map)) {
                return {cache.at(map), current_cycle - 1};
            }
            else {
                cache[map] = current_cycle;
            }
        }
        throw std::runtime_error("Cycle not found");
    };

    const std::pair<gsl::index,gsl::index> cycle_pair = find_cycle(map);
    const gsl::index cycle_num = nth_cycle_equivalent_to(cycle_pair, no_of_cycles);

    const RockMap resulting_map = [&](){
        for (const auto& pair : cache) {
            if (pair.second == cycle_num) {
                return pair.first;
            }
        }
        throw std::runtime_error("Map not found in cache");
    }();

    return resulting_map;
}

// load calculation

unsigned rows_from_south_edge(gsl::index row_ix, size_t no_of_rows){
    return no_of_rows - row_ix;
};

unsigned calc_load(const RockMap& map) {
    const std::vector<Position> rolling_stone_positions = calc_rolling_stones_positions(map);
    return std::accumulate(cbegin(rolling_stone_positions), cend(rolling_stone_positions), 0u, [&](unsigned acc, const Position& position){
        return acc + rows_from_south_edge(position.row_ix, map.size());
    });
}

// answers

unsigned part_1(RockMap map) {
    tilt(map, Direction::up());
    return calc_load(map);
}

unsigned part_2(RockMap map) {
    const auto final_map = map_after_n_cycles(std::move(map), 1'000'000'000);
    return calc_load(final_map);
}

int main() {
    const RockMap map = parse("../rocks.txt");
    fmt::print("Part 1: {}\n", map);
    fmt::print("Part 2: {}\n", map);
    return 0;
}