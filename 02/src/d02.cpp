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

unsigned long long get_multiplier(std::string const& str) {
    auto const length = str.length();
    return static_cast<unsigned long long>(std::pow(10, (length % 2 == 0) ? length / 2 : length / 2 + 1));
}

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./02/input"s;
    auto const filepath = base / filename;

    unsigned long long result = 0;

    std::ifstream stream(filepath);
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }
    std::string line;
    std::getline(stream, line);
    std::size_t offset = 0;
    while (offset < line.size()) {
        auto const end_first = line.find('-', offset);
        auto const first = line.substr(offset, end_first - offset);
        offset = end_first + 1;
        auto const end_last = line.find(',', offset);
        auto const last = line.substr(offset, end_last - offset);
        offset = end_last;
        if (offset != std::string::npos) {
            ++offset;
        }

        auto const lower_bound = std::stoull(std::string(first));
        auto const upper_bound = std::stoull(std::string(last));

        auto begin = first.substr(0, first.length()/2);
        if (first.length() % 2 != 0) {
            begin = std::string(first.length()/2 + 1, '0');
            begin[0] = '1';
        }

        auto half_value = std::stoull(begin);
        auto length = static_cast<unsigned long long>(std::log10(half_value)) + 1;
        auto multiplier = static_cast<unsigned long long>(std::pow(10, length));
        auto value = half_value * multiplier + half_value;
        if (value < lower_bound) {
            ++half_value;
            length = static_cast<unsigned long long>(std::log10(half_value)) + 1;
            multiplier = static_cast<unsigned long long>(std::pow(10, length));
            value = half_value * multiplier + half_value;
        }

        while (value <= upper_bound) {
            result += value;
            ++half_value;
            length = static_cast<unsigned long long>(std::log10(half_value)) + 1;
            multiplier = static_cast<unsigned long long>(std::pow(10, length));
            value = half_value * multiplier + half_value;
        }
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
    expect(solve_problem_1("example"s), 1227775554ull);
    std::cout << solve_problem_1("first"s) << std::endl;

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
