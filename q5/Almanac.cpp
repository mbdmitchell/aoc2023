#include "Almanac.h"

std::vector<unsigned long> Almanac::calc_seeds_old() const {
    std::vector<unsigned long> seeds_long_vec;
    std::transform(cbegin(tokenized_data[0]) + 1, cend(tokenized_data[0]),
                   back_inserter(seeds_long_vec), [](const std::string& str) {
        return str_to_num<unsigned long>(str);
    });
    return seeds_long_vec;
}

std::vector<Interval> Almanac::calc_seeds_new() const {
    std::vector<Interval> new_seeds;
    for (size_t i = 0; i < seeds_old.size(); i += 2) {
        Interval seed_range = {seeds_old[i], seeds_old[i] + seeds_old[i + 1] - 1};
        new_seeds.emplace_back(seed_range);
    }
    std::sort(begin(new_seeds), end(new_seeds), [&](const auto& a, const auto& b){ return a.lower() < b.lower(); });
    return new_seeds;
}

std::vector<Source_Destination_Range> Almanac::get_x_almanac_map(const std::string &first_token_of_title) const {
    std::vector<Source_Destination_Range> map;
    const auto start = std::find_if(cbegin(tokenized_data), cend(tokenized_data), [&](const auto& sub_vec){
        return sub_vec.front() == first_token_of_title;
    }) + 1;
    const auto finish = std::find_if(start, cend(tokenized_data), [&](const auto& sub_vec){
        return sub_vec.size() == 1; // empty row is counted as 1 using std::boost::split
    });
    for (auto row_itr = start; row_itr < finish; ++row_itr) {
        const auto destination = str_to_num<long>((*row_itr)[0]);
        const auto source = str_to_num<long>((*row_itr)[1]);
        const auto r = str_to_num<long>((*row_itr)[2]);
        const Source_Destination_Range sdr = {
            .range = {source, source + r - 1},
            .jump_distance = destination - source
        };
        map.push_back(sdr);
    }
    std::sort(begin(map), end(map), [&](const auto& a, const auto& b){ return a.range.lower() < b.range.lower(); });
    return map;
}


unsigned long Almanac::loc_to_loc(unsigned long thing, const AlmanacMap &map) {
    for (const auto& entry : map) {
        if (boost::numeric::in(thing, entry.range)) {
            return thing + entry.jump_distance;
        }
    }
    return thing;
}

Almanac::Almanac(std::istream &data)
        : tokenized_data{tokenize(raw_vector<std::string>(data))}
        , seeds_old{calc_seeds_old()}
        , seeds_new{calc_seeds_new()}
        , maps({get_x_almanac_map("seed-to-soil")
                , get_x_almanac_map("soil-to-fertilizer")
                , get_x_almanac_map("fertilizer-to-water")
                , get_x_almanac_map("water-to-light")
                , get_x_almanac_map("light-to-temperature")
                , get_x_almanac_map("temperature-to-humidity")
                , get_x_almanac_map("humidity-to-location")})
{ }

unsigned long Almanac::seed_to_location(unsigned long seed) const {
    for (const auto& map : maps) {
        seed = loc_to_loc(seed, map);
    }
    return seed;
}

std::vector<Interval> Almanac::split_seed_ranges_based_on_map(const std::vector<Interval> &seed_ranges,
                                                              const Almanac::AlmanacMap &almanac_map) {
    /// Split the seed Intervals so that each interval satisfies either of the following properties:
    /// 1. All seed numbers within the interval map to distinct values on the almanac_map, or
    /// 2. None of the seed numbers within the interval map to distinct values on the almanac_map

    std::vector<Interval> seeds_that_map = calc_seeds_that_map(seed_ranges, almanac_map);

    std::vector<Interval> seeds_that_dont_map = [&](){
        const Interval seed_span = {seed_ranges.front().lower(), seed_ranges.back().upper()};
        const std::vector<Interval> map_intervals = AlmanacMap_to_IntervalVector(almanac_map);
        try {
            const std::vector<Interval> map_complement = complement_intervals(seed_span, map_intervals);
            return calc_seeds_that_map(seed_ranges, map_complement);
        }
        catch (std::runtime_error& empty_interval_created) {
            return std::vector<Interval>{};
        }
    }();

    const std::vector<Interval> seeds = [&](){
        auto s = std::move(seeds_that_map);
        s.insert(
            end(seeds),
            std::make_move_iterator(begin(seeds_that_dont_map)),
            std::make_move_iterator(end(seeds_that_dont_map))
        );
        std::sort(begin(s), end(s), [&](const auto& a, const auto& b){ return a.lower() < b.lower(); });
        return s;
    }();

    return seeds;

}

std::vector<Interval>
Almanac::calc_seeds_that_map(const std::vector<Interval>& seed_ranges, const Almanac::AlmanacMap& almanac_map) {
    return calc_seeds_that_map(seed_ranges, AlmanacMap_to_IntervalVector(almanac_map));
}

std::vector<Interval>
Almanac::calc_seeds_that_map(const std::vector<Interval>& seed_ranges, const std::vector<Interval>& map_vec) {
    std::vector<Interval> stm;
    for (const auto& s_range : seed_ranges) {
        for (const auto& m_range : map_vec) {
            try {
                const Interval itv = boost::numeric::intersect(m_range, s_range);
                stm.push_back(itv);
            }
            catch (...) { } // if `itv` is empty interval it throws
        }
    }
    return stm;
}

std::vector<Interval> Almanac::AlmanacMap_to_IntervalVector(const Almanac::AlmanacMap& almanac_map) {
    std::vector<Interval> map_intervals;
    for (const auto& info : almanac_map) {
        map_intervals.push_back(info.range);
    }
    return map_intervals;
}

std::vector<Interval> Almanac::seed_range_vector_to_location() const {
    std::vector<Interval> ranges = seeds_new;
    for (const auto& map : maps) {
        std::sort(begin(ranges), end(ranges), [](const auto& a, const auto& b){ return a.lower() < b.lower(); });
        ranges = split_seed_ranges_based_on_map(ranges, map);
        ranges = pass_ranges_through_map(ranges, map);
    }
    return ranges;
}

std::vector<Interval>
Almanac::pass_ranges_through_map(const std::vector<Interval>& ranges, const Almanac::AlmanacMap& almanac_map) {

    std::vector<Interval> new_ranges;

    for (const auto& range : ranges){

        auto map_match = std::find_if(cbegin(almanac_map), cend(almanac_map),[&](const Source_Destination_Range& sdr) {
            return boost::numeric::in(range.lower(), sdr.range); // NB: any num in `range` should work
        });

        if (map_match == cend(almanac_map)) {
            new_ranges.push_back(range);
        }
        else {
            const long& jump_distance = map_match->jump_distance;
            const Interval result = (jump_distance >= 0)
                    ? static_cast<unsigned long>(jump_distance) + range
                    : range - static_cast<unsigned long>(-jump_distance);
            new_ranges.push_back(result);
        }
    }

    return new_ranges;
}

std::vector<unsigned long> Almanac::final_p1_seeds_locations() const {
    auto seeds = seeds_old;
    std::transform(cbegin(seeds), cend(seeds), begin(seeds), [&](auto &s) { return seed_to_location(s); });
    return seeds;
}
