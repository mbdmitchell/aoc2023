#pragma once
#include "Utils.h"
#include <map>

class Universe {
public:
    using Location = std::pair<size_t, size_t>;
private:
    UniverseMap universe_map;
    MultiplierMap multiplier_map;
    static UniverseMap parse(std::string_view file_name);

    // multiplier_map
    void reserve_and_set_to_one();
    void process_column(size_t col);
    void init_multiplier_map();

    [[nodiscard]] unsigned calc_expansion_rows(Location src, Location dst) const;
    [[nodiscard]] unsigned calc_expansion_cols(Location src, Location dst) const;

    [[nodiscard]] unsigned long long part_n_solution(int EXPANSION_MULTIPLIER) const;

public:

    [[nodiscard]] std::vector<Location> get_galaxy_locations() const;
    [[nodiscard]] unsigned shortest_path(Location src, Location dst, int EXPANSION_MULTIPLIER) const;

    explicit Universe(std::string_view file_name);

    [[nodiscard]] unsigned part_1_solution() const;
    [[nodiscard]] unsigned long part_2_solution() const;
};
