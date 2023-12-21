#include <iostream>
#include <fstream>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <numeric>
#include <vector>

// Custom classes

class InitializationStep {
    std::string m_str;

public:
    enum class Operation {DASH, EQUAL};
    explicit InitializationStep(std::string_view str) : m_str{str} {}

    [[nodiscard]] std::string label_str() const {
        auto end = std::find_if_not(cbegin(m_str), cend(m_str), isalpha);
        return {begin(m_str), end};
    }
    [[nodiscard]] Operation oper() const {
        const auto it = std::find_if_not(cbegin(m_str), cend(m_str), isalpha);
        switch (*it) {
            case '-': return Operation::DASH;
            case '=': return Operation::EQUAL;
            default: throw std::runtime_error("Invalid operation char");
        }
    }

    [[nodiscard]] const std::string& str() const { return m_str; }

    friend std::hash<InitializationStep>;
};

// std::hash implimentation for InitializationStep - overkill but I wanted to try it :)
namespace std { 
    template <>
    struct hash<InitializationStep> {
        size_t operator()(const std::string_view str) const {
            return std::accumulate(cbegin(str), cend(str), size_t{0}, [](size_t acc, char c) {
                return ((acc + static_cast<size_t>(c)) * 17) % 256;
            });
        }
    };
}

class Lens {
    std::string m_label;
    std::optional<unsigned> m_focal_length;
public:
    [[nodiscard]] size_t box_ix() const { return std::hash<InitializationStep>{}(m_label); }
    explicit Lens(const InitializationStep& is)
        : m_label{is.label_str()}
        , m_focal_length{
            (is.oper() == InitializationStep::Operation::EQUAL)
                ? static_cast<std::optional<unsigned>>(is.str().back() - '0')
                : std::nullopt}
        { }
    [[nodiscard]] const std::string& label() const { return m_label; }
    [[nodiscard]] std::optional<unsigned> focal_length() const { return m_focal_length; }
};

using Box = std::vector<Lens>;

// File handling

bool is_valid_file(std::string_view file_path){
    namespace fs = boost::filesystem;
    if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
        std::cerr << fmt::format("File does not exist or is not a regular file: {}\n", file_path);
        return false;
    }
    return true;
}

std::vector<InitializationStep> parse(std::string_view file_path) {

    assert(is_valid_file(file_path));

    std::ifstream file(file_path.data());
    if (!file.is_open()) {
        std::cerr << fmt::format("Unable to open file: {}\n", file_path);
        return {};
    }

    const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::vector<std::string> words;
        boost::split(words, content, boost::is_any_of(", "), boost::token_compress_on);

    std::vector<InitializationStep> initialization_steps;
        initialization_steps.reserve(words.size());
        for (const auto& word : words) {
            initialization_steps.emplace_back(word);
        }

    return initialization_steps;
}

unsigned part_1(const std::vector<InitializationStep>& vec) {
    return std::accumulate(cbegin(vec), cend(vec), 0u, [](unsigned acc, const InitializationStep& step) {
        return acc + std::hash<InitializationStep>{}(step.str());
    });
}

void follow_initialization_step(std::array<Box, 256>& boxes, const InitializationStep& step) {
    const Lens lens {step};
    auto& target_box = boxes[lens.box_ix()];
  
    const auto same_label = std::ranges::find_if(target_box, [&](const Lens& l){
        return l.label() == lens.label();
    });

    switch (step.oper()) {
        case InitializationStep::Operation::DASH : {
            if (same_label != end(target_box)) {
                target_box.erase(same_label);
            }
            return;
        }
        case InitializationStep::Operation::EQUAL : {
            if (same_label == end(target_box)) {
                target_box.push_back(lens);
            }
            else {
                *same_label = lens;
            }
            return;
        }
    }
}

unsigned calc_focusing_power(const std::array<Box, 256>& boxes) {
    unsigned total = 0;

    for (size_t box_ix = 0; box_ix < boxes.size(); ++box_ix) {
        const auto& current_box = boxes[box_ix];

        unsigned subtotal = 0;
        for (size_t len_ix = 0; len_ix < current_box.size(); ++len_ix) {
            const auto& current_lens = current_box[len_ix];
            subtotal += static_cast<unsigned>((box_ix + 1) * (len_ix + 1) * current_lens.focal_length().value());
        }
        
        total += subtotal;
    }
    return total;
}

unsigned part_2(const std::vector<InitializationStep>& steps) {
    std::array<Box, 256> boxes;
    for (const auto& step : steps) {
        follow_initialization_step(boxes, step);
    }
    return calc_focusing_power(boxes);
}

int main() {
    const std::vector<InitializationStep> data = parse("../input.txt");
    fmt::print("Part 1: {}\n", part_1(data));
    fmt::print("Part 2: {}\n", part_2(data));
    return 0;
}
