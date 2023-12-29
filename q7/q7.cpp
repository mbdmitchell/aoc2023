#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include "Hand.h"
#include "Utils.h"

[[nodiscard]] unsigned long calc_result(Part&& part, std::string_view str) {

    std::ifstream data(str);
    if (!data.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    const auto hands_and_bids = [&](){
        auto hands_and_bids = parse(data, &part);
        std::ranges::sort(hands_and_bids, [&](const Hand_And_Bid& a, const Hand_And_Bid& b) {
            return part.compare(a.hand, b.hand);
        });
        return hands_and_bids;
    }();

    const unsigned long result = [&](){
        unsigned long result = 0;
        for (size_t i = 0; i < hands_and_bids.size(); ++i) {
            const auto ranking = i + 1;
            result += (ranking * hands_and_bids[i].bid);
        }
        return result;
    }();

    return result;
}

int main() {

    const unsigned long part_1 = calc_result(P1(), "../hands.txt");
    const unsigned long part_2 = calc_result(P2(), "../hands.txt");

    fmt::print("Part 1: {}\n", part_1);
    fmt::print("Part 2: {}\n", part_2);

    return 0;
}
