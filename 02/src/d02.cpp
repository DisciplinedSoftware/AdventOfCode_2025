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

static unsigned long long make_repeated_value(unsigned long long half, size_t times) {
    std::string s = std::to_string(half);
    std::string total;
    total.reserve(s.size() * times);
    for (size_t i = 0; i < times; ++i) total += s;
    return std::stoull(total);
}


unsigned long long count_invalid_id(
    std::string const& first,
    unsigned long long lower_bound,
    unsigned long long upper_bound,
    size_t nb_repetitions,
    std::unordered_set<unsigned long long>& seen) {
    unsigned long long result = 0;

    auto begin = first.substr(0, first.length()/nb_repetitions);
    auto const nb_char_to_add = std::min(first.length() % nb_repetitions, 1ul);
    if (nb_char_to_add != 0) {
        begin = std::string(first.length()/nb_repetitions + nb_char_to_add, '0');
        begin[0] = '1';
    }

    auto half_value = std::stoull(begin);

    auto value = make_repeated_value(half_value, nb_repetitions);

    if (value < lower_bound) {
        ++half_value;
        value = make_repeated_value(half_value, nb_repetitions);
    }

    while (value <= upper_bound) {
        if (!seen.contains(value)) {
            seen.insert(value);
            result += value;
        }
        ++half_value;
        value = make_repeated_value(half_value, nb_repetitions);
    }

    return result;
}

unsigned long long count_invalid_id(std::string const& filename, size_t nb_max_repetitions) {
    std::filesystem::path const base = "./02/input"s;
    auto const filepath = base / filename;

    unsigned long long result = 0;
    std::unordered_set<unsigned long long> seen;

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

        result += count_invalid_id(first, lower_bound, upper_bound, 2, seen);

        for (size_t nb_repetitions = 3; nb_repetitions <= nb_max_repetitions && nb_repetitions <= last.length(); nb_repetitions += 2) {
            result += count_invalid_id(first, lower_bound, upper_bound, nb_repetitions, seen);
        }
    }

    return result;
}

unsigned long long solve_problem_1(std::string const& filename) {
    return count_invalid_id(filename, 2);
}

unsigned long long solve_problem_2(std::string const& filename) {
    return count_invalid_id(filename, std::numeric_limits<size_t>::max());
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

    expect(solve_problem_2("example"s), 4174379265ull);
    std::cout << solve_problem_2("first"s) << std::endl;
}
