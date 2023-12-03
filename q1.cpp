#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <numeric>
#include <cassert>

const std::map<std::string, int> word_to_int = {{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}};
const std::vector<std::string> number_words = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

struct Info {
    std::string::const_iterator pos;
    int val = -1;
};

// ... digit_info()

template <typename Compare>
Info n_most_digit_info(const std::string& row, Compare comp) {
    /// std::less<>() for leftmost, std::greater<>() for rightmost

    auto max_digit = row.cend();

    for (auto it = row.begin(); it != row.end(); ++it) {
        if (std::isdigit(*it, std::locale()) && (max_digit == row.end() || comp(it, max_digit))) {
            max_digit = it;
        }
    }

    return {
        .pos = max_digit,
        .val = (max_digit == row.cend()) ? -1 : *max_digit - '0'
    };

}

Info leftmost_digit_info(const std::string& row) { // FOR P1
    return n_most_digit_info(row, std::less<>());
}

Info rightmost_digit_info(const std::string& row) { // FOR P1
    return n_most_digit_info(row, std::greater<>());
}

// ... written_info()

template <typename Compare>
Info n_most_written_info(const std::string& row, Compare comp) {
    /// std::less<>() for leftmost, std::greater<>() for rightmost

    std::optional<std::string::size_type> n_most_pos{};
    int corresponding_int = -1;

    for (const auto& word : number_words) {
        std::string::size_type pos = 0;
        while (pos != std::string::npos) {
            pos = row.find(word, pos);
            if (pos != std::string::npos) {
                if (!n_most_pos.has_value() || comp(pos, n_most_pos)) {
                    n_most_pos = pos;
                    corresponding_int = word_to_int[word];
                }
                ++pos;
            }
        }
    }

    return {
        .pos = n_most_pos.has_value() ? cbegin(row) + static_cast<long>(n_most_pos.value()) : cend(row),
        .val = corresponding_int
    };

}

// _most()

template <typename Compare>
int n_most(const std::string& row, Compare compare) {
    /// std::less<>() for leftmost, std::greater<>() for rightmost

    const Info written_info = n_most_written_info(row, compare);
    const Info digit_info = n_most_digit_info(row, compare);

    assert((written_info.val != -1) || (digit_info.val != -1));
    if (written_info.val == -1) {
        return digit_info.val;
    }
    else if (digit_info.val == -1) {
        return written_info.val;
    }
    else {
        return compare(written_info.pos, digit_info.pos) ? written_info.val : digit_info.val;
    }

}

int leftmost(const std::string& row) {
    return n_most(row, std::less<>());
}

int rightmost(const std::string& row){
    return n_most(row, std::greater<>());
}

template<typename T>
std::vector<T> vectorize(std::istream& is) {
    std::vector<T> vec;
    T str;
    while(std::getline(is, str)){
        vec.push_back(str);
    }
    return vec;
}

int main() {

    std::ifstream data("./data.txt");
    if (data.bad()) {
        throw std::ifstream::failure("Unable to open file");
    }

    const std::vector<std::string> lines = vectorize<std::string>(data);

    const int part_1 = std::accumulate(cbegin(lines), cend(lines), 0, [&](int acc, const std::string& l){
        return acc + 10 * leftmost_digit_info(l).val + rightmost_digit_info(l).val;
    });

    const int part_2 = std::accumulate(cbegin(lines), cend(lines), 0, [&](int acc, const std::string& l){
        return acc + 10 * leftmost(l) + rightmost(l);
    });

    std::cout << part_1 << '\n';
    std::cout << part_2 << '\n';

    return 0;
}
