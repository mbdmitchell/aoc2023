#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include "DesertMap.h"

int main() {

    std::ifstream data("../maps.txt");
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    const DesertMap desert_map {data};

    const auto part_1 = desert_map.steps("AAA", false);
    const auto part_2 = desert_map.part_2_solution();

    fmt::print("Part 1: {}\n", part_1);
    fmt::print("Part 2: {}\n", part_2);

    return 0;
}
