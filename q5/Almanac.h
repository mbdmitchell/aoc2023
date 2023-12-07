#pragma once

#include <boost/numeric/interval.hpp>
#include <iostream>
#include "utils.h"

struct Source_Destination_Range {
    Interval range;
    long jump_distance;
};

class Almanac {
    using AlmanacMap = std::vector<Source_Destination_Range>;

    std::vector<std::vector<std::string>> tokenized_data;
    std::vector<unsigned long> seeds_old;
    std::vector<Interval> seeds_new;
    std::array<AlmanacMap, 7>  maps;

    [[nodiscard]] std::vector<unsigned long> calc_seeds_old() const;
    [[nodiscard]] std::vector<Interval> calc_seeds_new() const;
    [[nodiscard]] static std::vector<Interval> calc_seeds_that_map(const std::vector<Interval>& seed_ranges, const AlmanacMap& almanac_map);
    [[nodiscard]] static std::vector<Interval> calc_seeds_that_map(const std::vector<Interval>& seed_ranges, const std::vector<Interval>& map_vec);

    [[nodiscard]] std::vector<Source_Destination_Range> get_x_almanac_map(const std::string& first_token_of_title) const;

    // conversion
    [[nodiscard]] static std::vector<Interval> AlmanacMap_to_IntervalVector(const AlmanacMap& almanac_map);
    [[nodiscard]] static std::vector<Interval> split_seed_ranges_based_on_map(const std::vector<Interval>& seed_ranges, const AlmanacMap& almanac_map) ;

    // passing through maps
    [[nodiscard]] static unsigned long loc_to_loc(unsigned long thing, const AlmanacMap& map);
    [[nodiscard]] static std::vector<Interval> pass_ranges_through_map(const std::vector<Interval>& ranges, const Almanac::AlmanacMap &almanac_map);

public:
    explicit Almanac(std::istream& data);
    [[nodiscard]] std::vector<unsigned long> final_p1_seeds_locations() const;;
    [[nodiscard]] std::vector<Interval> seed_range_vector_to_location() const;
    [[nodiscard]] unsigned long seed_to_location(unsigned long seed) const;
};