#include <iostream>
#include <fstream>
#include "DesertMap.h"

int main() {

    std::ifstream data("../maps.txt");
    const DesertMap desert_map {data};

    const auto part_1_solution = desert_map.steps("AAA", false);
    const auto part_2_solution = desert_map.part_2_solution();

    std::cout << part_1_solution << '\n' << part_2_solution << '\n';
    return 0;
}
