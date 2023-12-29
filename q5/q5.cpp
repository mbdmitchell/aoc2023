#include <fmt/format.h>
#include <fstream>
#include <map>
#include "Almanac.h"

int main() {

    std::ifstream data("../almanac.txt");
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    const Almanac almanac {data};

    const unsigned long part_1 = [&](){
        const auto locations = almanac.final_p1_seeds_locations();
        return *std::ranges::min_element(locations);
    }();

    const unsigned long part_2 = [&](){
        const std::vector<Interval> final_ranges = almanac.seed_range_vector_to_location();
        return std::ranges::min_element(final_ranges, {}, &Interval::lower)->lower();
    }();

    fmt::print("Part 1: {}\n", part_1);
    fmt::print("Part 2: {}\n", part_2);

    return 0;
}
