#include <iostream>
#include <numeric>

#include "DesertMap.h"

DesertMap::DesertMap(std::istream &data)
        : vectorize_data{vectorize<std::string>(data)}
        , move_cycle{vectorize_each_char(vectorize_data[0])}
        , source_to_lr_destinations{extract_map()}
{
    vectorize_data.clear();
    vectorize_data.shrink_to_fit();
}

DesertMap::Source_And_LR_Map DesertMap::extract_map() const {

    const auto tokenized = [&](){
        std::vector<std::vector<std::string>> tokenized = tokenize(vectorize_data);
        for (size_t i = 1; i < tokenized.size(); ++i) { // can skip tokenized[0]
            for (auto &str: tokenized[i]) {
                boost::range::remove_erase_if(str, boost::is_any_of("(),"));
            }
        }
        return tokenized;
    }();

    const auto mp = [&](){
        Source_And_LR_Map mp;
        for (size_t i = 2; i < tokenized.size(); ++i) {
            mp[tokenized[i][0]] = {tokenized[i][2], tokenized[i][3]};
        }
        return mp;
    }();

    return mp;
}

uint64_t DesertMap::part_2_solution() const {

    const std::vector<unsigned> steps_for_each_source = [&](){
        std::vector<unsigned> s;
        for (auto &[key, val]: source_to_lr_destinations){
            if (key[2] == 'A'){
                s.emplace_back(DesertMap::steps(key, true));
            }
        }
        return s;
    }();

    const uint64_t multiple = [&](){
        uint64_t m = 1;
        for (auto& step: steps_for_each_source){
            m = std::lcm(m, step);
        }
        return m;
    }();

    return multiple;

}

unsigned DesertMap::steps(std::string pos, bool is_ghost) const {
    unsigned step = 0;

    while ((!is_ghost && pos != "ZZZ") || (is_ghost && pos[2] != 'Z')){
        const auto& next = source_to_lr_destinations.at(pos);
        pos = (move_cycle[step % move_cycle.size()] == 'L') ? next.first : next.second;
        ++step;
    }

    return step;
}
