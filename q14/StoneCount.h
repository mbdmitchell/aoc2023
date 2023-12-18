#pragma once

#include "Position.h"

struct StoneCount {
    Position stone_position;
    unsigned no_of_stones; // no_of_stones_pushing_on_that_stone
    friend bool operator==(const StoneCount& sc, const StoneCount& other);
};

bool operator==(const StoneCount& sc, const StoneCount& other) {
    return sc.stone_position == other.stone_position && sc.no_of_stones == other.no_of_stones;
}