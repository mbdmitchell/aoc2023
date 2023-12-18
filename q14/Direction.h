#pragma once

class Direction {
    int m_row_delta;
    int m_col_delta;
    Direction(int row_delta, int col_delta);
public:
    static Direction up();
    static Direction down();
    static Direction left();
    static Direction right();

    friend bool operator==(const Direction& dir, const Direction& other);

    [[nodiscard]] int row_delta() const { return m_row_delta; }
    [[nodiscard]] int col_delta() const { return m_col_delta; }
};


