#include <iostream>
#include "Universe.h"

int main() {
    const Universe universe = Universe("../star_map.txt");
    std::cout << universe.part_1_solution() << '\n' << universe.part_2_solution() << '\n';
    return 0;
}
