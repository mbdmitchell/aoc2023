#include "Hand.h"

std::array<CardCount, 13> Hand::calc_card_count() const {
    auto no_of_card_value = [&](char c) -> unsigned {
        return std::count_if(cbegin(hand), cend(hand), [&](const char& hand_c){ return c == hand_c; });
    };
    return std::array<CardCount, 13> {
            CardCount{.count = no_of_card_value('2'), .type = '2'},
            CardCount{.count = no_of_card_value('3'), .type = '3'},
            CardCount{.count = no_of_card_value('4'), .type = '4'},
            CardCount{.count = no_of_card_value('5'), .type = '5'},
            CardCount{.count = no_of_card_value('6'), .type = '6'},
            CardCount{.count = no_of_card_value('7'), .type = '7'},
            CardCount{.count = no_of_card_value('8'), .type = '8'},
            CardCount{.count = no_of_card_value('9'), .type = '9'},
            CardCount{.count = no_of_card_value('T'), .type = 'T'},
            CardCount{.count = no_of_card_value('J'), .type = 'J'},
            CardCount{.count = no_of_card_value('Q'), .type = 'Q'},
            CardCount{.count = no_of_card_value('K'), .type = 'K'},
            CardCount{.count = no_of_card_value('A'), .type = 'A'}
    };
}

Hand::Kind Hand::calc_kind(const CardCount &highest, const CardCount &second_highest) {
    if (highest.count == 5) {
        return Kind::FIVE_OF_A_KIND;
    } else if (highest.count == 4) {
        return Kind::FOUR_OF_A_KIND;
    } else if (highest.count == 3) {
        if (second_highest.count == 2) { return Kind::FULL_HOUSE; }
        else { return Kind::THREE_OF_A_KIND; }
    } else if (highest.count == 2) {
        if (second_highest.count == 2) { return Kind::TWO_PAIR; }
        else { return Kind::ONE_PAIR; }
    } else {
        return Kind::HIGH_CARD;
    }
}

Hand::Kind Hand::calc_kind(const Part *move_strategy) const {
    std::array<CardCount, 13> count = calc_card_count();
    return move_strategy->best_move(count);
}

Hand::Hand(std::string_view hand, const Part *move_strategy) : hand{hand}, kind{calc_kind(move_strategy)} {}


Hand::Kind P2::best_move(std::array<CardCount, 13> card_count) const {

    std::sort(begin(card_count), end(card_count), [](const CardCount& a, const CardCount& b){ return a.count < b.count; });

    auto is_wildcard = [](const CardCount& cc){
        return cc.type == 'J';
    };

    const auto j_count = std::ranges::find_if(card_count, [&](const CardCount& cc) { return is_wildcard(cc); })->count; // what if no j's??????

    const auto highest_not_j_iter = std::find_if_not(crbegin(card_count), crend(card_count), [&](const CardCount& cc) {
        return is_wildcard(cc);
    });
    const auto second_highest_not_j_iter = std::find_if_not(highest_not_j_iter + 1, crend(card_count), [&](const CardCount& cc) {
        return is_wildcard(cc);
    });

    const CardCount highest = {highest_not_j_iter->count + j_count, highest_not_j_iter->type};
    const CardCount second_highest = {second_highest_not_j_iter->count, second_highest_not_j_iter->type};

    return Hand::calc_kind(highest, second_highest);
}

P2::P2() : Part("J23456789TQKA") {}

Hand::Kind P1::best_move(std::array<CardCount, 13> card_count) const {

    std::sort(begin(card_count), end(card_count), [](const CardCount& a, const CardCount& b){ return a.count < b.count; });

    const auto highest = *(card_count.cend() - 1);
    const auto second_highest = *(card_count.cend() - 2);

    return Hand::calc_kind(highest, second_highest);
}

P1::P1() : Part("23456789TJQKA") {}

bool Part::compare(const Hand &a, const Hand &b) const {

    if (a.get_kind() != b.get_kind()) {
        return a.get_kind() < b.get_kind();
    }
    const auto& a_hand = a.get_hand();
    const auto& b_hand = b.get_hand();

    for (size_t i = 0; i < a_hand.size(); ++i) {
        if (a_hand[i] == b_hand[i]) continue;
        return order.find(a_hand[i]) < order.find(b_hand[i]);
    }

    return false;
}

bool operator<(const CardCount &a, const CardCount &b) { return a.count < b.count; }
