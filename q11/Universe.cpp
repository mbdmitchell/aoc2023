#include "Universe.h"

UniverseMap Universe::parse(std::string_view file_name) {
    std::ifstream data(file_name);
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
    UniverseMap m = tokenize(vectorize<std::string>(data));
    return m;
}

void Universe::reserve_and_set() {
    multiplier_map.reserve(universe_map.size());
    for (size_t row = 0; row < universe_map.size(); ++row) {
        multiplier_map.emplace_back();
        multiplier_map[row].reserve(universe_map[row].size());
        multiplier_map[row].resize(universe_map[row].size(), 0);
    }
}

void Universe::process_column(size_t col) {
    for (size_t row = 0; row < universe_map.size(); ++row) {
        if (universe_map[row][col] == '#') { return; }
    }
    for (size_t row = 0; row < multiplier_map.size(); ++row) {
        multiplier_map[row][col] = 1;
    }
}

void Universe::init_multiplier_map() {
    reserve_and_set();
    for (size_t row = 0; row < universe_map.size(); ++row) {
        if (std::ranges::none_of(universe_map[row], [](const char c) { return c == '#'; })) {
            std::ranges::fill(multiplier_map[row], 1);
        }
    }
    for (size_t col = 0; col < universe_map.front().size(); ++col) {
        process_column(col);
    }
}

Universe::Universe(std::string_view file_name) : universe_map{parse(file_name)} {
    init_multiplier_map();
}

std::vector<Universe::Location> Universe::get_galaxy_locations() const {
    std::vector<Location> galaxy_locations;
    for (size_t row = 0; row < universe_map.size(); ++row) {
        for (size_t col = 0; col < universe_map[row].size(); ++col) {
            if (universe_map[row][col] == '#') {
                galaxy_locations.emplace_back(row, col);
            }
        }
    }
    return galaxy_locations;
}

unsigned Universe::calc_expansion_rows(Universe::Location src, Universe::Location dst) const {
    if (src.first > dst.first) {
        std::swap(src, dst);
    }
    unsigned count = 0;
    for (size_t row = src.first; row < dst.first; ++row) {
        if (multiplier_map[row].front() == 1) {
            ++count;
        }
    }
    return count;
}

unsigned Universe::calc_expansion_cols(Universe::Location src, Universe::Location dst) const {
    if (src.second > dst.second) {
        std::swap(src, dst);
    }
    unsigned count = 0;
    for (size_t col = src.second; col < dst.second; ++col) {
        if (multiplier_map.front()[col] == 1) {
            ++count;
        }
    }
    return count;
}


unsigned Universe::shortest_path(Universe::Location src, Universe::Location dst, int EXPANSION_MULTIPLIER) const {
    const auto expansion_rows = calc_expansion_rows(src, dst);
    const auto expansion_cols = calc_expansion_cols(src, dst);
    const auto total_rows = expansion_rows*EXPANSION_MULTIPLIER + std::abs((int)dst.first - (int)src.first) - expansion_rows; // use static_cast
    const auto total_cols = expansion_cols*EXPANSION_MULTIPLIER + std::abs((int)dst.second - (int)src.second) - expansion_cols; // use static_cast
    return total_rows + total_cols;
}

unsigned Universe::part_1_solution() const {
    return part_n_solution(2);
}

unsigned long Universe::part_2_solution() const {
    return part_n_solution(1'000'000);
}

unsigned long long Universe::part_n_solution(int EXPANSION_MULTIPLIER) const {
    unsigned long long total = 0;
    const auto galaxies = get_galaxy_locations();

    for (size_t i1 = 0; i1 < galaxies.size() - 1; ++i1) {
        for (size_t i2 = i1 + 1; i2 < galaxies.size(); ++i2) {
            total += shortest_path(galaxies[i1], galaxies[i2], EXPANSION_MULTIPLIER);
        }
    }
    return total;
}
