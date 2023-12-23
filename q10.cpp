#include <cassert>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>

// TYPES

using SheepMap = std::vector<std::vector<char>>;

using Location = std::pair<size_t, size_t>;
enum class Direction {UP, DOWN, LEFT, RIGHT};

Location operator+(const Location& coord, const Direction& direction) {
    switch (direction) {
        case Direction::UP: return {coord.first - 1, coord.second};
        case Direction::DOWN: return {coord.first + 1, coord.second};
        case Direction::LEFT: return {coord.first, coord.second - 1};
        case Direction::RIGHT: return {coord.first, coord.second + 1};
    }
}

// FILE PARSING

template<typename T>
[[nodiscard]] SheepMap tokenize(const std::vector<T>& lines_of_data) {

    SheepMap tokenized_lines_of_data;
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

SheepMap parse(std::string_view file_name) {
    std::ifstream data(file_name);
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    return tokenize(vectorize<std::string>(data));
}

// HELPER FUNCTION(S)

Location find_S(const SheepMap& map) { // std::pair<row, col>
    for (size_t row = 0; row < map.size(); ++row){
        for (size_t col = 0; col < map.front().size(); ++col){
            if (map[row][col] == 'S') {
                return {row, col};
            }
        }
    }
    throw std::runtime_error("No S found");
}

Direction opposite_of(const auto& direction){
    switch (direction) {
        case Direction::UP: return Direction::DOWN;
        case Direction::DOWN: return Direction::UP;
        case Direction::LEFT: return Direction::RIGHT;
        case Direction::RIGHT: return Direction::LEFT;
    }
}

const std::map<char, std::pair<Direction, Direction>> valid_directions_from_pipe {
        {'|', {Direction::UP, Direction::DOWN}}
        , {'-', {Direction::LEFT, Direction::RIGHT}}
        , {'L', {Direction::UP, Direction::RIGHT}}
        , {'J', {Direction::UP, Direction::LEFT}}
        , {'F', {Direction::DOWN, Direction::RIGHT}}
        , {'7', {Direction::DOWN, Direction::LEFT}}
};

void update_max(std::optional<unsigned>& current_max, std::optional<unsigned> candidate) {
    if (!candidate.has_value()) return;
    current_max = !current_max.has_value() ? candidate.value() : std::max(current_max.value(), candidate.value());
}

// PATH TRAVERSAL

struct PathTravellerData {
    Location starting_location;
    Direction travel_direction;
    SheepMap map;
    bool requires_path_points;
};

std::tuple<std::optional<unsigned>, std::vector<Location>> path_traveller(PathTravellerData data) {
    /// starting_location: must be neighbour of S (up, down, left, or right)

    Location current = data.starting_location;
    char char_at_dest_square;
    Direction opposite_direction;

    unsigned count = 1;

    std::vector<Location> all_path_points;

    do {
        all_path_points.push_back(current);
        current = current + data.travel_direction;
        char_at_dest_square = data.map[current.first][current.second];

        if (char_at_dest_square == '.') {
            return {std::nullopt, all_path_points};
        }
        if (char_at_dest_square == 'S') {
            if (data.requires_path_points){
                all_path_points.push_back(current);
            }
            return {count + 1, all_path_points};
        }

        const auto dir_pair = valid_directions_from_pipe.at(char_at_dest_square);

        opposite_direction = opposite_of(data.travel_direction);
        data.travel_direction = (dir_pair.first == opposite_direction) ? dir_pair.second : dir_pair.first;
        ++count;

    } while (current != data.starting_location);

    return {std::nullopt, all_path_points};

}

std::optional<Direction> calc_next_direction(Location start, Direction direction, const SheepMap& map) {
    std::pair<Direction,Direction> possible_directions;
    try {
        possible_directions = valid_directions_from_pipe.at(map[start.first][start.second]);
    }
    catch (std::out_of_range& err) { // key not found
        return std::nullopt;
    }
    return (possible_directions.first == opposite_of(direction))
           ? possible_directions.second
           : possible_directions.first;
}

// PART 2 FUNCTIONS

std::vector<Location> calc_path_points(const SheepMap& map){

    const Location start_location = find_S(map);

    std::optional<unsigned> max_length_loop = std::nullopt;
    std::optional<Direction> max_direction = std::nullopt;

    for (const auto dir : {Direction::RIGHT, Direction::UP, Direction::DOWN, Direction::LEFT}) {

        const auto candidate_start_position = start_location + dir;

        const auto first_step_direction = calc_next_direction(candidate_start_position, dir, map);
        if (!first_step_direction.has_value()) continue;

        std::optional<unsigned> path_length = get<0>(path_traveller({
            .starting_location = candidate_start_position
            , .travel_direction = first_step_direction.value()
            , .map = map
            , .requires_path_points = false
        }));

        const auto prev = max_length_loop;

        update_max(max_length_loop, path_length);

        if (prev != max_length_loop) {
            max_direction = dir;
        }
    }

    const Location start_position = start_location + max_direction.value();
    const Direction direction = max_direction.value();

    const std::vector<Location> path_points = get<1>(path_traveller({
        .starting_location = start_position
        , .travel_direction = calc_next_direction(start_position, direction, map).value()
        , .map = map
        , .requires_path_points = true
    }));

    return path_points;

}

unsigned points_in_polygon(const std::vector<Location>& points) {
    /// Find the area of the loop using shoelace formula, then run pick's theorem in reverse.
    int sum = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        const Location n_1 = points[i];
        const Location n_2 = points[(i+1) % points.size()];
        const auto& row_1 = n_1.first;
        const auto& col_1 = n_1.second;
        const auto& row_2 = n_2.first;
        const auto& col_2 = n_2.second;
        sum += col_1 * row_2 - row_1 * col_2;
    }

    const unsigned area = std::abs(sum/2);

    return area - points.size()/2 + 1;
}

// SOLUTION FUNCTIONS

unsigned calc_part_1(const SheepMap& map){

    const Location start_location = find_S(map);
    std::optional<unsigned> max_length_loop = std::nullopt;

    for (const auto dir : {Direction::RIGHT, Direction::UP, Direction::DOWN, Direction::LEFT}) {

        const auto candidate_start_position = start_location + dir;
        const auto first_step_direction = calc_next_direction(candidate_start_position, dir, map);

        if (!first_step_direction.has_value()) continue;

        std::optional<unsigned> path_length = get<0>(path_traveller({
            .starting_location = candidate_start_position
            , .travel_direction = first_step_direction.value()
            , .map = map
            , .requires_path_points = false
        }));

        update_max(max_length_loop, path_length);
    }

    return max_length_loop.value() / 2;
}

unsigned calc_part_2(const SheepMap& map) {
    return points_in_polygon(calc_path_points(map));
}

int main() {
    const SheepMap map = parse("../map.txt");
    fmt::print("Part 1: {}\n", calc_part_1(map));
    fmt::print("Part 2: {}\n", calc_part_2(map));
    return 0;
}
