#pragma once

#include <map>
#include "Utils.h"
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

using SourceDestDistancesMap = std::map<std::pair<std::string,std::string>, std::optional<unsigned>>;

class DesertMap {
    using Source_And_LR_Map = std::map<std::string, std::pair<std::string, std::string>>;
    std::vector<std::string> vectorize_data; // USED FOR INITIALIZATION THEN FREED
    std::vector<char> move_cycle;
    Source_And_LR_Map source_to_lr_destinations;
    SourceDestDistancesMap all_start_to_all_dest_lengths;

    [[nodiscard]] std::vector<std::string> get_locations_ending_with(char c) const;
    [[nodiscard]] Source_And_LR_Map extract_map() const;
public:
    [[nodiscard]] unsigned steps(std::string pos, bool ghost) const;
    [[nodiscard]] uint64_t part_2_solution() const;
    explicit DesertMap(std::istream& data);
};

