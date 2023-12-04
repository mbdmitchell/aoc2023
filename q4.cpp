#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // for boost::split
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>

using IntVec = std::vector<int>;
using StrVec = std::vector<std::string>;
using Card = std::pair<IntVec, IntVec>;
using CardWithStrVals = std::pair<StrVec, StrVec>;

// type conversions

template<typename T>
std::vector<T> vectorize(std::istream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

std::vector<StrVec> tokenize(const StrVec& lines_of_data) {
    std::vector<StrVec> tokenized_lines_of_data;
    for (const auto& line : lines_of_data) {
        StrVec tokens;
        boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
        tokenized_lines_of_data.push_back(tokens);
    }
    return tokenized_lines_of_data;
}

IntVec str_to_int_vec(const StrVec& vec) {
    IntVec i_vec;
    for (auto& num : vec) {
        i_vec.push_back(stoi(num));
    }
    return i_vec;
}

bool is_num_token(std::string_view token) {
    /// helper function for split_str_vec()
    return std::all_of(cbegin(token), cend(token), [](const char c){ return isdigit(c); });
}

CardWithStrVals split_str_vec(const StrVec& vec) {
    const auto it = std::find_if_not(cbegin(vec), cend(vec), is_num_token);
    const StrVec part1 = std::vector(cbegin(vec), it);
    const StrVec part2 = std::vector(it+1, cend(vec));
    return {part1, part2};
}

void remove_leading_n_elements_from_each_sub_vector(std::vector<StrVec>& vec, size_t n) {
    /// helper function for convert_data()
    for (auto& sub_vec : vec) {
        sub_vec.erase(begin(sub_vec), begin(sub_vec) + static_cast<int>(n));
    }
}

std::vector<Card> convert_data(std::ifstream& data) {
    /// Unaltered input data --> vector of cards
    /// Each vector element is a card, a card is a pair of {winning nums, your nums}, respectively

    // -> [line{strA,strA2,strA3,strA4,...,strAn}, ..., lineN{strN,strN2,strN3,strN4,...,strNn}]
    // eg. line: {"84", "73", "|", "24", "20"}
    const std::vector<StrVec> tokenized_lines_of_data = [&](){
        std::vector<std::string> lines_of_data = vectorize<std::string>(data);
        std::vector<StrVec> tokenized_lines_of_data = tokenize(lines_of_data);
        remove_leading_n_elements_from_each_sub_vector(tokenized_lines_of_data, 2);
        return tokenized_lines_of_data;
    }();

    // convert data to str cards
    // -> [{StrVec0, StrVec'0}, ..., {StrVecn, StrVec'n}]
    const std::vector<CardWithStrVals> split_vec = [&tokenized_lines_of_data](){
        std::vector<CardWithStrVals> vec;
        for (auto& str_vec : tokenized_lines_of_data) {
            vec.push_back(split_str_vec(str_vec));
        }
        return vec;
    }();

    // convert str cards to `Card`s
    // -> [{IntVec0, IntVec'0}, ..., {IntVecn, IntVec'n}]
    const std::vector<Card> winning_and_your_numbers = [&](){
        std::vector<Card> w;
        for (const auto& str_vec_pair : split_vec) {
            w.emplace_back(str_to_int_vec(str_vec_pair.first), str_to_int_vec(str_vec_pair.second));
        }
        return w;
    }();

    return winning_and_your_numbers;
}

// calc functions

int calc_card_score(int no_of_matches) {
    if (no_of_matches == 0) return 0;
    return 1 << (no_of_matches-1); // == 1 ^ no_of_matches
}

int calc_card_matches(const Card& card) {
    const IntVec& winning_nums = card.first;
    const IntVec& your_nums = card.second;
    const int matches = std::accumulate(cbegin(winning_nums), cend(winning_nums), 0, [&](int acc, const auto w_num){
        return acc + std::any_of(cbegin(your_nums), cend(your_nums), [&](int y_num){ return y_num == w_num; });
    });
    return matches;
}

int calc_total_score(const std::vector<Card>& winning_and_your_numbers) {
    const auto& wayn = winning_and_your_numbers;
    return std::accumulate(cbegin(wayn), cend(wayn), 0, [&](int acc, const auto& card){
        return acc + calc_card_score(calc_card_matches(card));
    });
}

std::map<size_t, int> calc_win_map(const std::vector<Card>& cards) {
    std::map<size_t, int> iw;
    for (size_t i = 0; i < cards.size(); ++i) {
        iw[i] = calc_card_matches(cards[i]);
    }
    return iw;
}

std::vector<int> calc_card_count(const std::vector<int>& initial_count, const std::map<size_t, int>& wins_on_card) {
    assert(wins_on_card.size() == initial_count.size());
    std::vector<int> card_count = initial_count;
    for (size_t i = 0; i < card_count.size(); ++i) {
        const auto no_of_matches = wins_on_card.at(i);
        for (size_t m = 1; m <= no_of_matches; ++m) {
            if (i+m >= card_count.size()) continue;
            card_count[i+m] += card_count[i];
        }
    }
    return card_count;
}

int main() {

    std::ifstream data("../data.txt");
    if (data.bad()) {
        throw std::ifstream::failure("Unable to open file");
    }

    const std::vector<Card> cards = convert_data(data);
    const std::map<size_t, int> wins_on_card = calc_win_map(cards);

    const std::vector<int> card_count = [&](){
        const auto initial_count = std::vector<int>(wins_on_card.size(), 1);
        return calc_card_count(initial_count, wins_on_card);
    }();

    const int sol_1 = calc_total_score(cards);
    const int sol_2 = std::accumulate(cbegin(card_count), cend(card_count), 0);

    std::cout << sol_1 << ' ' << sol_2 << '\n';
}
