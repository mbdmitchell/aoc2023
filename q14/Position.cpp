#include "Position.h"

bool operator==(const Position& pos, const Position& other) {
    return pos.row_ix == other.row_ix && pos.col_ix == other.col_ix;
}

std::size_t PositionHash::operator()(const Position &pos) const {
    return std::hash<gsl::index>()(pos.row_ix) ^ (std::hash<gsl::index>()(pos.col_ix) << 1);
}
