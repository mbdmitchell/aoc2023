#include "Direction.h"

Direction::Direction(int row_delta, int col_delta) : m_row_delta{row_delta}, m_col_delta{col_delta} {}

Direction Direction::up() { return {-1, 0}; } // -1 'cause visually row 0 is at the top
Direction Direction::down() { return {1,0}; }
Direction Direction::left() {return {0,-1}; }
Direction Direction::right() {return {0, 1}; }

bool operator==(const Direction& dir, const Direction& other) {
    return dir.m_row_delta == other.m_row_delta && dir.m_col_delta == other.m_col_delta;
}