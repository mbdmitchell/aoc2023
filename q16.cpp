#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fstream>
#include <gsl/gsl>
#include <iostream>
#include <numeric>
#include <set>
#include <vector>

// Custom classes -- TODO: move to seperate files

struct Location {
    gsl::index row, col;
    friend bool operator==(Location direction, Location other);
    auto operator<=>(const Location& other) const { 
        if (row != other.row) {
            return row <=> other.row;
        }
        else {
            return col <=> other.col;
        }
    }
};

bool operator==(Location location, Location other) {
    return location.row == other.row && location.col == other.col;
}

class Direction {
    int row_delta, col_delta;
    Direction(int row_delta, int col_delta) : row_delta{row_delta}, col_delta{col_delta} { }
public:
    [[nodiscard]] static Direction up() { return {-1, 0}; } // -1 as, visually, row 0 is at the top
    [[nodiscard]] static Direction down() { return {1, 0}; }
    [[nodiscard]] static Direction left() { return {0, -1}; }
    [[nodiscard]] static Direction right() { return {0, 1}; }
    friend Location operator+(Location location, Direction direction);
    friend bool operator==(Direction direction, Direction other);
    auto operator<=>(const Direction& other) const { // all this for std::set later... worth it?
        if (row_delta != other.row_delta) {
            return row_delta <=> other.row_delta;
        }
        else {
            return col_delta <=> other.col_delta;
        }
    }
};

bool operator==(Direction direction, Direction other) {
    return direction.row_delta == other.row_delta && direction.col_delta == other.col_delta;
}
Location operator+(Location location, Direction direction) {
    return {location.row + direction.row_delta, location.col + direction.col_delta};
}

struct Beam {
    Location location = {0,0};
    Direction direction = Direction::right();
    auto operator<=>(const Beam& other) const {
        if (location != other.location) {
            return location <=> other.location;
        } else {
            return direction <=> other.direction;
        }
    }
};

class Square {
public:
    enum class Type {EMPTY, SPLITTER_HORIZONTAL, SPLITTER_VERTICAL, MIRROR_FORWARDSLASH, MIRROR_BACKSLASH};
private:
    Type to_type(char sqr) {
        switch (sqr) {
            case '.' : return Type::EMPTY;
            case '-' : return Type::SPLITTER_HORIZONTAL;
            case '|' : return Type::SPLITTER_VERTICAL;
            case '/' : return Type::MIRROR_FORWARDSLASH;
            case '\\': return Type::MIRROR_BACKSLASH;
            default: throw std::invalid_argument("Invalid char");
        }
    }
    Type m_type;
    bool m_is_illuminated = false;
public:
    explicit Square(char sym)  : m_type{to_type(sym)} {}
    [[nodiscard]] Type type() const { return m_type; }
    void illuminate() { m_is_illuminated = true; }
    void delluminate() { m_is_illuminated = false;}
    bool is_illuminated() const {return m_is_illuminated; }
};

class Grid {
    using Type = Square::Type;

    std::vector<std::vector<Square>> grid;
    std::vector<Beam> beams {Beam{}};
    std::set<Beam> cache; // store prev beams to exit early if in loop

    void handle_forward_mirror(Beam& beam) { // todo - feel there is a smarter way of expressing this
        if (beam.direction == Direction::up()) {
            beam.direction = Direction::right();
        }
        else if (beam.direction == Direction::right()) {
            beam.direction = Direction::up();
        }
        else if (beam.direction == Direction::down()) {
            beam.direction = Direction::left();
        }
        else {
            beam.direction = Direction::down();
        }
        beam.location = beam.location + beam.direction;
    }
    void handle_backward_mirror(Beam& beam) {
        if (beam.direction == Direction::up()) {
            beam.direction = Direction::left();
        }
        else if (beam.direction == Direction::right()) {
            beam.direction = Direction::down();
        }
        else if (beam.direction == Direction::down()) {
            beam.direction = Direction::right();
        }
        else {
            beam.direction = Direction::up();
        }
        beam.location = beam.location + beam.direction;
    }

    void move_beams_to_next_location() {
    /// NB: happily moves to off-grid / invalid locations

        std::vector<Beam> beams_from_split; // seperated to avoid changing beams.size() while iterating through

        // helpers
        auto handle_horizontal_splitter = [&](Beam& beam, gsl::index i) -> void {
            const bool is_split = beam.direction == Direction::up() || beam.direction == Direction::down();
            if (!is_split) {
                beam.location = beam.location + beam.direction;
            }
            else {
                Beam left_beam = {beam.location + Direction::left(), Direction::left()};
                Beam right_beam = {beam.location + Direction::right(), Direction::right()};

                beams.erase(begin(beams) + i);            // remove original ... 
                beams_from_split.push_back(left_beam);    // and create two beams
                beams_from_split.push_back(right_beam);
            }
        };
        auto handle_vertical_splitter = [&](Beam& beam, gsl::index i) -> void {
            const bool is_split = beam.direction == Direction::left() || beam.direction == Direction::right();
            if (!is_split) {
                beam.location = beam.location + beam.direction;
            }
            else {
                Beam up_beam = {beam.location + Direction::up(), Direction::up()};
                Beam down_beam = {beam.location + Direction::down(), Direction::down()};

                beams.erase(begin(beams) + i);           // remove original ... 
                beams_from_split.push_back(up_beam);     // and create two beams
                beams_from_split.push_back(down_beam);
            }
        };

        for (gsl::index i = 0; i < beams.size(); ++i) {
            auto& beam = beams[i];
            const Type beam_loc_type = grid[beam.location.row][beam.location.col].type();
            switch (beam_loc_type) {
                case Type::EMPTY : {
                    beam.location = beam.location + beam.direction;
                    break;
                }
                case Type::MIRROR_FORWARDSLASH : {
                    handle_forward_mirror(beam);
                    break;
                }
                case Type::MIRROR_BACKSLASH : {
                    handle_backward_mirror(beam);
                    break;
                }
                case Type::SPLITTER_HORIZONTAL : {
                    const auto& old_size = beams.size();
                    handle_horizontal_splitter(beam, i);
                    if (beams.size() != old_size) {
                        --i; // counteract deletion in handle_horizontal_splitter
                    }
                    break;
                }
                case Type::SPLITTER_VERTICAL : {
                    const auto& old_size = beams.size();
                    handle_vertical_splitter(beam, i);
                    if (beams.size() != old_size) {
                        --i; // counteract deletion in handle_vertical_splitter
                    }
                    break;
                }
            }
        }

        if (!beams_from_split.empty()){
            beams.reserve(beams.size() + beams_from_split.size());
            std::move(beams_from_split.begin(), beams_from_split.end(), std::back_inserter(beams));
        }
    }

    void remove_unecessary_beams() {
        const auto grid_rows = grid.size();
        const auto grid_cols = grid[0].size();

        std::erase_if(beams, [&](const Beam& b) {
            return cache.contains(b) ||
            !(0 <= b.location.row && b.location.row < grid_rows &&
            0 <= b.location.col && b.location.col < grid_cols);
        });
    }

    void illuminate_squares() {
        /// Assumes valid grid locations
        for (const Beam& beam : beams) {
            grid[beam.location.row][beam.location.col].illuminate();
        }
    }

    void delluminate_all() {
        for (auto& row : grid) {
            for (auto& col : row) {
                col.delluminate();
            }
        }
    }

public:

    Grid(const std::vector<std::vector<Square>>& grid) : grid{grid} { }

    void init_illumination_process() {
      
        cache.clear();  
        delluminate_all();
        remove_unecessary_beams();

        for (const auto& b : beams) { // add starting beam
            cache.insert(b);
        }

        illuminate_squares();

        while (!beams.empty()) {
            move_beams_to_next_location();
            remove_unecessary_beams();
            illuminate_squares();
            for (const auto& b : beams) {
                cache.insert(b);
            }
        }
    }

    unsigned no_of_illuminated_squares() const {
        assert(cache.size() != 0 && "Forgot to init_illumination_process()");
        return std::accumulate(cbegin(grid), cend(grid), size_t{0}, [](size_t acc, const std::vector<Square>& row) {
            return acc + std::ranges::count_if(row, [](const Square& square){ return square.is_illuminated(); });
        });
    }

    void set_starting_beam(const Beam& b){
        beams.clear(); 
        beams.push_back(b);
    }

    std::vector<Beam> get_starting_beams() const {
        std::vector<Beam> beams;

        for (gsl::index i = 0; i < grid.size(); ++i) {
            const gsl::index max_row = gsl::narrow_cast<gsl::index>(grid.size()-1);
            const gsl::index max_col = gsl::narrow_cast<gsl::index>(grid[0].size()-1);

            beams.emplace_back(Location{i, 0}, Direction::right()); // left edge
            beams.emplace_back(Location{i, max_col}, Direction::left()); // right edge
            beams.emplace_back(Location{0, i}, Direction::down()); // top edge
            beams.emplace_back(Location{max_row, i}, Direction::up()); // bottom edge
        }

        return beams;
    }

};

// File Handling

bool is_valid_file(std::string_view file_path){
    namespace fs = boost::filesystem;
    if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
        fmt::print(stderr, "File does not exist or is not a regular file: {}\n", file_path);
        return false;
    }
    return true;
}

auto tokenize(const std::vector<std::string> &lines_of_data) {
    std::vector<std::vector<Square>> tokenized_lines_of_data;
    for (const auto& line : lines_of_data) {
        const std::vector<Square> tokens = [&](){
            std::vector<Square> tokens;
            for (char c : line) {
                tokens.emplace_back(c);
            }
            return tokens;
        }();
        tokenized_lines_of_data.push_back(tokens);
    }
    return tokenized_lines_of_data;
}

template<typename T>
[[nodiscard]] std::vector<T> vectorize(std::ifstream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

Grid parse(std::string_view file_path) {
    assert(is_valid_file(file_path));
    std::ifstream data(file_path);
    return Grid{tokenize(vectorize<std::string>(data))};
}

// solutions

unsigned part_1(Grid grid) {
    grid.init_illumination_process();
    return grid.no_of_illuminated_squares();
}

unsigned part_2(Grid grid) {
    const std::vector<Beam> starting_beams = grid.get_starting_beams();

    auto get_illuminated_squares = [&](const Beam& b) -> unsigned {
        grid.set_starting_beam(b);
        grid.init_illumination_process();
        return grid.no_of_illuminated_squares();
    };

    std::vector<unsigned> scores(starting_beams.size());
    std::transform(cbegin(starting_beams), cend(starting_beams), begin(scores), get_illuminated_squares);

    unsigned max_squares = *std::ranges::max_element(scores);

    return max_squares;
}

int main() {
    Grid grid = parse("../input.txt");
    fmt::print("Part 1: {}\n", part_1(grid)); 
    fmt::print("Part 2: {}\n", part_2(grid)); 
    return 0;
}
