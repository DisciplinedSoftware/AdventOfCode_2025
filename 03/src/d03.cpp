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

inline unsigned long long to_value(char c) {
    return static_cast<unsigned long long>(c - '0');
}

inline unsigned long long to_joltage(unsigned long long v1, unsigned long long v2) {
    return v1 * 10 + v2;
}

unsigned long long get_max_joltage(std::string const& str) {
    unsigned long long max_joltage = 0;
    auto current_max_digit = to_value(str[0]);

    for (auto c : str | std::views::drop(1)) {
        auto const v = to_value(c);
        auto const current_joltage = to_joltage(current_max_digit, v);
        current_max_digit = std::max(current_max_digit, v);
        max_joltage = std::max(max_joltage, current_joltage);
    }

    return max_joltage;
}

unsigned long long get_max_joltage(std::string const& str, unsigned long long nb_batteries) {
    unsigned long long max_joltage = 0;
    auto begin = str.begin();
    for (size_t i = 1; i <= nb_batteries; ++i) {
        auto const it = std::max_element(begin, str.end() - (nb_batteries - i));
        max_joltage = max_joltage * 10 + to_value(*it);
        begin = it + 1;
    }

    return max_joltage;
}

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./03/input"s;
    auto const filepath = base / filename;

    unsigned long long sum = 0;

    std::ifstream stream(filepath);
    std::string line;
    while (std::getline(stream, line) || !line.empty()) {
        sum += get_max_joltage(line, 2);
    }

    return sum;
}

unsigned long long solve_problem_2(std::string const& filename) {
    std::filesystem::path const base = "./03/input"s;
    auto const filepath = base / filename;

    unsigned long long sum = 0;

    std::ifstream stream(filepath);
    std::string line;
    while (std::getline(stream, line) || !line.empty()) {
        sum += get_max_joltage(line, 12);
    }

    return sum;
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
    expect(get_max_joltage("987654321111111"s), 98ull);
    expect(get_max_joltage("811111111111119"s), 89ull);
    expect(get_max_joltage("234234234234278"s), 78ull);
    expect(get_max_joltage("818181911112111"s), 92ull);

    expect(solve_problem_1("example"s), 357ull);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(get_max_joltage("987654321111111"s, 12), 987654321111ull);
    expect(get_max_joltage("811111111111119"s, 12), 811111111119ull);
    expect(get_max_joltage("234234234234278"s, 12), 434234234278ull);
    expect(get_max_joltage("818181911112111"s, 12), 888911112111ull);

    expect(solve_problem_2("example"s), 3121910778619ull);
    std::cout << solve_problem_2("first"s) << std::endl;
}
