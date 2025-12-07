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
    next_beams.reserve(beams.size());

    while (std::getline(stream, line) && !line.empty()) {
        for (auto beam : beams) {
            if (line[beam] == '^') {
                ++result;
                if (beam != 0) {
                    next_beams.emplace(beam-1);
                }
                if (beam != line.size()-1) {
                    next_beams.emplace(beam+1);
                }
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

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
