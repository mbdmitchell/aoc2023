#include "utils.h"

std::vector<Interval> complement_intervals(const Interval& largeInterval, std::vector<Interval> smallIntervals) {

    std::vector<Interval> complement;

    if (smallIntervals.empty()) {
        complement.push_back(largeInterval);
        return complement;
    }

    auto nudge = [](Interval& iv) {
        const unsigned long lower_increment = iv.lower() == 0 ? 0 : 1;
        const unsigned long upper_increment = iv.upper() == std::numeric_limits<unsigned long>::max() ? 0 : 1;
        iv = Interval{iv.lower() + lower_increment, iv.upper() + upper_increment};
    };

    for (auto& inter : smallIntervals) {
        nudge(inter); // *
    }

    unsigned long current = largeInterval.lower();

    for (const auto& s_interval : smallIntervals) {
        if (s_interval.lower() > current) {
            complement.emplace_back(current, s_interval.lower());
        }
        current = std::max(current, s_interval.upper());
    }

    if (current < largeInterval.upper()) {
        complement.emplace_back(current, largeInterval.upper());
    }

    return complement;
    // * without nudge, ranges and the complement overlap, eg:
    //   ranges: 6-15, 30-45, 50-55, 60-70, 80-90,...
    //   complm: 1-6,  15-30, 45-49, 55-60, 70-80,...
}
