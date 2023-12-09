#pragma once

#include <iostream>

struct CardCount {
    unsigned count;
    char type;
};

bool operator< (const CardCount& a, const CardCount& b);

class Part;

class Hand { // so can override <, >
public:
    enum class Kind {HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, FULL_HOUSE, FOUR_OF_A_KIND, FIVE_OF_A_KIND};

    explicit Hand(std::string_view hand, const Part* move_strategy);
    std::array<CardCount, 13> calc_card_count() const;
    const std::string& get_hand() const { return hand; }
    const Kind& get_kind() const {return kind; }

    static Kind calc_kind(const CardCount& highest, const CardCount& second_highest);
    Kind calc_kind(const Part* move_strategy) const;
private:
    std::string hand;
    Kind kind;
};

struct Hand_And_Bid {
    Hand hand;
    int bid;
};

class Part {
protected:
    std::string order;
public:
    Part(const std::string& ord) : order{ord} {}
    virtual Hand::Kind best_move(std::array<CardCount, 13> card_count) const = 0;
    bool compare(const Hand& a, const Hand& b) const;
};

class P1 : public Part {
public:
    P1();
    Hand::Kind best_move(std::array<CardCount, 13> card_count) const override;
};

class P2 : public Part {
public:
    P2();
    Hand::Kind best_move(std::array<CardCount, 13> card_count) const override;
};

