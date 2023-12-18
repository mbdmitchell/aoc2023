#pragma once

#include <cstddef> // for size_t
#include <gsl/gsl>

struct Position {
    gsl::index row_ix, col_ix;
    friend bool operator==(const Position& pos, const Position& other);
};

struct PositionHash {
    std::size_t operator()(const Position& pos) const;
};

