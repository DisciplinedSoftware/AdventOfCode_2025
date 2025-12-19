#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <generator>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std::string_literals;

std::generator<std::pair<char, int>> parse_input(std::string const& filename) {
    std::filesystem::path const base = "./01/input"s;
    auto const filepath = base / filename;

    std::ifstream stream(filepath);
    std::string line;
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }
    while (std::getline(stream, line) || !line.empty()) {
        char direction = line[0];
        int steps = std::stoi(line.substr(1));
        co_yield {direction, steps};
    }
}

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./01/input"s;
    auto const filepath = base / filename;

    int dial = 50;
    unsigned long long counter = 0;

    for (auto const [direction, steps] : parse_input(filename)) {
        if (direction == 'L') {
            dial += 100 - (steps % 100);
        }
        else if (direction == 'R') {
            dial += steps;
        }
        else {
            throw std::runtime_error("Invalid direction");
        }

        dial %= 100;
        if (dial == 0) {
            ++counter;
        }
    }

    return counter;
}

unsigned long long solve_problem_2(std::string const& filename) {
    std::filesystem::path const base = "./01/input"s;
    auto const filepath = base / filename;

    int dial = 50;
    unsigned long long counter = 0;

    for (auto [direction, steps] : parse_input(filename)) {
        counter += steps / 100;
        steps %= 100;

        if (direction == 'L') {
            dial -= steps;
        }
        else if (direction == 'R') {
            dial += steps;
        }
        else {
            throw std::runtime_error("Invalid direction");
        }

        if (std::abs(dial) != steps && (dial <= 0 || dial >= 100)) {
            ++counter;
        }

        dial = (dial + 100) % 100;
    }

    return counter;
}

void expect(auto const & result, auto const & reference) {
    if (result == reference) {
        std::cout << "Success" << std::endl;
    }
    else {
        std::cout << "Failure: " << result << " != " << reference << std::endl;
    }
}

int main() {
    expect(solve_problem_1("example"s), 3u);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 6u);
    std::cout << solve_problem_2("first"s) << std::endl;
}
