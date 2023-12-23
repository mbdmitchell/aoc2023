#include <fmt/format.h>
#include <iostream>
#include "Universe.h"

int main() {
    const Universe universe = Universe("../star_map.txt");
    fmt::print("Part 1: {}\n", universe.part_1_solution());
    fmt::print("Part 2: {}\n", universe.part_2_solution());
    return 0;
}
