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
    std::filesystem::path const base = "./07/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    unsigned long long result = 0;

    std::string line;
    if (!std::getline(stream, line)) {
        throw std::runtime_error("Could not read from file: "s + filepath.string());
    }

    std::unordered_set<size_t> beams;
    beams.reserve(line.size());
    beams.emplace(line.find('S'));
    std::unordered_set<size_t> next_beams;
    next_beams.reserve(line.size());

    while (std::getline(stream, line) && !line.empty()) {
        for (auto beam : beams) {
            if (line[beam] == '^') {
                ++result;
                next_beams.emplace(beam-1);
                next_beams.emplace(beam+1);
            }
            else {
                next_beams.emplace(beam);
            }
        }

        std::swap(beams, next_beams);
        next_beams.clear();
    }

    return result;
}

unsigned long long solve_problem_2(std::string const& filename) {
    std::filesystem::path const base = "./07/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    std::string line;
    if (!std::getline(stream, line)) {
        throw std::runtime_error("Could not read from file: "s + filepath.string());
    }

    std::unordered_map<size_t, unsigned long long> beams;
    beams.reserve(line.size());
    beams.emplace(line.find('S'), 1);
    std::unordered_map<size_t, unsigned long long> next_beams;
    next_beams.reserve(line.size());

    while (std::getline(stream, line) && !line.empty()) {
        for (auto [beam, count] : beams) {
            if (line[beam] == '^') {
                next_beams[beam-1] += count;
                next_beams[beam+1] += count;
            }
            else {
                next_beams[beam] += count;
            }
        }

        std::swap(beams, next_beams);
        next_beams.clear();
    }

    return std::ranges::fold_left(beams, 0ull, [](unsigned long long acc, auto const& value) { return acc + value.second; });
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
    expect(solve_problem_1("example"s), 21);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 40);
    std::cout << solve_problem_2("first"s) << std::endl;
}
