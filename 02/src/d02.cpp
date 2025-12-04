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

static unsigned long long make_repeated_value(unsigned long long half, size_t times) {
    std::string s = std::to_string(half);
    std::string total;
    total.reserve(s.size() * times);
    for (size_t i = 0; i < times; ++i) total += s;
    return std::stoull(total);
}

static std::generator<std::pair<std::string,std::string>> get_data(std::string const& filename) {
    std::filesystem::path const base = "./02/input"s;
    auto const filepath = base / filename;

    std::ifstream stream(filepath);
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }

    std::string line;
    if (!std::getline(stream, line)) {
        throw std::runtime_error("Unable to read line from file: " + filepath.string());
    }

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
        co_yield {first, last};
    }
}

unsigned long long count_invalid_id(
    std::string const& first,
    unsigned long long lower_bound,
    unsigned long long upper_bound,
    size_t nb_repetitions,
    std::unordered_set<unsigned long long>& seen) {
    assert (nb_repetitions > 0 || lower_bound <= upper_bound);

    unsigned long long result = 0;

    size_t const base_half_len = first.length() / nb_repetitions;
    size_t const extra_char = std::min(first.length() % nb_repetitions, size_t(1));
    size_t const half_len = base_half_len + extra_char;

    std::string half_str;
    if (extra_char != 0) {
        half_str.assign(half_len, '0');
        half_str[0] = '1';
    } else {
        half_str = first.substr(0, half_len);
    }

    unsigned long long half_value = std::stoull(half_str);

    auto value = make_repeated_value(half_value, nb_repetitions);

    // advance until we are within or above the lower bound
    while (value < lower_bound) {
        ++half_value;
        value = make_repeated_value(half_value, nb_repetitions);
    }

    // iterate and accumulate unseen values within range
    while (value <= upper_bound) {
        auto insert_result = seen.insert(value);
        if (insert_result.second) result += value; // only add when newly inserted
        ++half_value;
        value = make_repeated_value(half_value, nb_repetitions);
    }

    return result;
}

unsigned long long count_invalid_id(
    std::string const& first,
    std::string const& last,
    size_t nb_max_repetitions,
    std::unordered_set<unsigned long long>& seen) {

    unsigned long long result = 0;

    auto const lower_bound = std::stoull(first);
    auto const upper_bound = std::stoull(last);

    result += count_invalid_id(first, lower_bound, upper_bound, 2, seen);

    for (size_t nb_repetitions = 3; nb_repetitions <= nb_max_repetitions && nb_repetitions <= last.length(); nb_repetitions += 2) {
        result += count_invalid_id(first, lower_bound, upper_bound, nb_repetitions, seen);
    }
    return result;
}

unsigned long long count_invalid_id(std::string const& filename, size_t nb_max_repetitions) {
    unsigned long long result = 0;
    std::unordered_set<unsigned long long> seen;

    for (auto const& [first, last] : get_data(filename)) {
        result += count_invalid_id(first, last, nb_max_repetitions, seen);
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
    {
        std::unordered_set<unsigned long long> seen;
        expect(count_invalid_id("11", "22", 2, seen), 33ull);                           seen.clear();
        expect(count_invalid_id("95", "115", 2, seen), 99ull);                          seen.clear();
        expect(count_invalid_id("998", "1012", 2, seen), 1010ull);                      seen.clear();
        expect(count_invalid_id("1188511880", "1188511890", 2, seen), 1188511885ull);   seen.clear();
        expect(count_invalid_id("222220", "222224", 2, seen), 222222ull);               seen.clear();
        expect(count_invalid_id("1698522", "1698528", 2, seen), 0ull);                  seen.clear();
        expect(count_invalid_id("446443", "446449", 2, seen), 446446ull);               seen.clear();
        expect(count_invalid_id("38593856", "38593862", 2, seen), 38593859ull);         seen.clear();
        expect(count_invalid_id("565653", "565659", 2, seen), 0ull);                    seen.clear();
        expect(count_invalid_id("824824821", "824824827", 2, seen), 0ull);              seen.clear();
        expect(count_invalid_id("2121212118", "2121212124", 2, seen), 0ull);            seen.clear();
    }

    expect(solve_problem_1("example"s), 1227775554ull);
    std::cout << solve_problem_1("first"s) << std::endl;


    {
        auto const nb_max_repetitions = std::numeric_limits<size_t>::max();
        std::unordered_set<unsigned long long> seen;
        expect(count_invalid_id("11", "22", nb_max_repetitions, seen), 33ull);                           seen.clear();
        expect(count_invalid_id("95", "115", nb_max_repetitions, seen), 210ull);                         seen.clear();
        expect(count_invalid_id("998", "1012", nb_max_repetitions, seen), 2009ull);                      seen.clear();
        expect(count_invalid_id("1188511880", "1188511890", nb_max_repetitions, seen), 1188511885ull);   seen.clear();
        expect(count_invalid_id("222220", "222224", nb_max_repetitions, seen), 222222ull);               seen.clear();
        expect(count_invalid_id("1698522", "1698528", nb_max_repetitions, seen), 0ull);                  seen.clear();
        expect(count_invalid_id("446443", "446449", nb_max_repetitions, seen), 446446ull);               seen.clear();
        expect(count_invalid_id("38593856", "38593862", nb_max_repetitions, seen), 38593859ull);         seen.clear();
        expect(count_invalid_id("565653", "565659", nb_max_repetitions, seen), 565656ull);               seen.clear();
        expect(count_invalid_id("824824821", "824824827", nb_max_repetitions, seen), 824824824ull);      seen.clear();
        expect(count_invalid_id("2121212118", "2121212124", nb_max_repetitions, seen), 2121212121ull);   seen.clear();
    }

    expect(solve_problem_2("example"s), 4174379265ull);
    std::cout << solve_problem_2("first"s) << std::endl;
}
