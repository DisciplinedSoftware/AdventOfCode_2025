#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
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

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./01/input"s;
    auto const filepath = base / filename;

    int dial = 50;
    unsigned long long counter = 0;

    std::ifstream stream(filepath);
    std::string line;
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }
    while (std::getline(stream, line) || !line.empty()) {
        std::istringstream line_stream(line);
        auto steps = std::stoi(line.substr(1));
        if (line[0] == 'L') {
            dial -= steps;
        }
        else if (line[0] == 'R') {
            dial += steps;
        }
        else {
            throw std::runtime_error("Invalid direction");
        }

        dial = (dial + 100) % 100;
        if (dial == 0) {
            ++counter;
        }
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
}
