#include <iostream>
#include <fstream>
#include <numeric>
#include <cassert>

using std::literals::string_literals::operator""s;

// helpers...

template<typename T>
std::vector<T> vectorize(std::istream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

int to_int(char c) {
    assert(isdigit(c,std::locale()));
    return c - '0';
}

// the rest...

enum class Colour {N, RED, GREEN, BLUE};

struct RGB_Count {
    int red, green, blue;
};

void update_rgb(RGB_Count& rgb, Colour colour, int total) {
    switch (colour) {
        case Colour::RED: rgb.red = std::max(rgb.red, total); break;
        case Colour::GREEN: rgb.green = std::max(rgb.green, total); break;
        case Colour::BLUE: rgb.blue = std::max(rgb.blue, total); break;
        default: break;
    }
}

void reset(int& total, int& multi) {
    total = 0; multi = 1;
}

// line parser ...

RGB_Count calc_max_of_each_colour(const std::string& line) {

    RGB_Count rgb {0, 0, 0};
    Colour current_colour = Colour::N;

    int total = 0, multi = 1;

    for (size_t i = line.length() - 1; i != std::numeric_limits<size_t>::max(); --i) { // i == std::numeric_limits<size_t>::max() when underflow.

        const char current = line[i];

        if (current == ':') break;

        if ("0123456789rgb"s.find(current) == std::string::npos) continue;

        if (isdigit(current)) {
            total += to_int(current) * multi;
            multi *= 10;
        }
        else if (total != 0) {
            update_rgb(rgb, current_colour, total);
            reset(total, multi);
        }

        switch (current) {
            case 'r': current_colour = Colour::RED; break;
            case 'g': current_colour = Colour::GREEN; break;
            case 'b': current_colour = Colour::BLUE; break;
            default: break;
        }
    }

    update_rgb(rgb, current_colour, total);
    return rgb;
}

bool is_valid(const RGB_Count& hand, const RGB_Count max) {
    return max.red >= hand.red
        && max.green >= hand.green
        && max.blue >= hand.blue;
}

int main() {

    std::ifstream data("../data.txt");
    if (data.bad()) {
        throw std::ifstream::failure("Unable to open file");
    }
    
    std::vector<std::string> lines = vectorize<std::string>(data);

    const int answer1 = [&](){

        const RGB_Count no_of_cubes {12, 13, 14};

        int sum_of_indices = 0;
        for (size_t i = 0; i < lines.size(); ++i) {
            const RGB_Count min_hand = calc_max_of_each_colour(lines[i]);
            sum_of_indices += is_valid(min_hand, no_of_cubes) ? static_cast<int>(i + 1) : 0;
        }
        return sum_of_indices;

    }();

    const int answer2 = [&](){
        return std::accumulate(cbegin(lines), cend(lines), 0, [&](int acc, const std::string& line){
            const RGB_Count min_hand = calc_max_of_each_colour(line);
            const int power = min_hand.red * min_hand.green * min_hand.blue;
            return acc + power;
        });
    }();

    std::cout << answer1 << '\n';
    std::cout << answer2 << '\n';

    return 0;
}
