#include <fstream>
#include <map>
#include "Almanac.h"

int main() {

    /*std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);*/

    std::ifstream data("../almanac.txt");
    if (data.bad()) { throw std::ifstream::failure("Unable to open file"); }

    const Almanac almanac {data};

    const unsigned long part_1 = [&](){
        const auto locations = almanac.final_p1_seeds_locations();
        return *std::min_element(cbegin(locations), cend(locations));
    }();

    const unsigned long part_2 = [&](){
        const std::vector<Interval> final_ranges = almanac.seed_range_vector_to_location();
        return std::min_element(begin(final_ranges), end(final_ranges), [](const auto& a, const auto& b){
            return a.lower() < b.lower();
        })->lower();
    }();

    std::cout << part_1 << '\n';
    std::cout << part_2 << '\n';

    return 0;
}
