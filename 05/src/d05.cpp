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

/*
    Look if an ingredient ID is in a range of fresh IDs.
    This can be done by checking each range one by one.
    Ranges can be sorted which would take O(n log n + log n * m) time
*/

/*
    Intervals needs to be merged first
    1. x--x
        xx
       start2 is less or equal to end1, merge
    2. xx
        xx
       end1 is greater or equal to start2, merge
    3. xx
         xx
       No need to do anything in this case, although it would speed up searching later on.
*/


struct Interval {
    unsigned long long begin;
    unsigned long long end;
};

std::ifstream open_input_file(std::string const& filename) {
    std::filesystem::path const base = "./05/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }
    return stream;
}

std::generator<Interval> read_intervals(std::ifstream& stream) {
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        auto const separator = line.find('-');
        auto const begin = std::stoull(line.substr(0, separator));
        auto const end = std::stoull(line.substr(separator + 1));
        co_yield Interval{begin, end};
    }
}

std::generator<unsigned long long> read_ingredient_ids(std::ifstream& stream) {
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        auto const ingredient_id = std::stoull(line);
        co_yield ingredient_id;
    }
}

void sort_and_merge_intervals(std::vector<Interval>& intervals) {
    if (intervals.size() <= 1) {
        return;
    }

    std::ranges::sort(intervals, std::less<>{},
        [](auto const& range) { return range.begin; });

    std::vector<Interval> merged;
    merged.reserve(intervals.size());

    auto next_it = intervals.begin() + 1;
    Interval current = intervals.front();
    while (next_it != intervals.end()) {
        if (next_it->begin <= current.end) {
            current.end = std::max(current.end, next_it->end);
        } else {
            merged.push_back(current);
            current = *next_it;
        }
        ++next_it;
    }

    merged.push_back(current);

    intervals = std::move(merged);
}

bool is_ingredient_fresh(unsigned long long ingredient_id, std::vector<Interval> const& fresh_intervals) {
    if (ingredient_id < fresh_intervals.front().begin || fresh_intervals.back().end < ingredient_id) {
        return false;
    }

    auto const it = std::ranges::lower_bound(fresh_intervals, ingredient_id, {},
        [](Interval const& interval) {
            return interval.end;
        });
    return it->begin <= ingredient_id && ingredient_id <= it->end;
}

unsigned long long solve_problem_1(std::string const& filename) {
    std::ifstream stream = open_input_file(filename);

    std::vector<Interval> fresh_intervals;
    for (auto&& intervals : read_intervals(stream)) {
        fresh_intervals.push_back(std::forward<Interval>(intervals));
    }

    sort_and_merge_intervals(fresh_intervals);

    return std::ranges::count_if(read_ingredient_ids(stream), 
        [&fresh_intervals](unsigned long long ingredient_id) {
            return is_ingredient_fresh(ingredient_id, fresh_intervals);
        });
}

unsigned long long solve_problem_2(std::string const& filename) {
    std::ifstream stream = open_input_file(filename);

    std::vector<Interval> fresh_intervals;
    for (auto&& intervals : read_intervals(stream)) {
        fresh_intervals.push_back(std::forward<Interval>(intervals));
    }

    sort_and_merge_intervals(fresh_intervals);

    return std::ranges::fold_left(fresh_intervals, 0ull,
        [](unsigned long long acc, Interval const& interval) {
            return acc + (interval.end - interval.begin + 1);
        });
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
    std::vector<Interval> fresh_intervals = {
        {3, 5},
        {10, 14},
        {16, 20},
        {12, 18}
    };
    sort_and_merge_intervals(fresh_intervals);
    expect(is_ingredient_fresh(1, fresh_intervals), false);
    expect(is_ingredient_fresh(3, fresh_intervals), true);
    expect(is_ingredient_fresh(5, fresh_intervals), true);
    expect(is_ingredient_fresh(8, fresh_intervals), false);
    expect(is_ingredient_fresh(11, fresh_intervals), true);
    expect(is_ingredient_fresh(17, fresh_intervals), true);
    expect(is_ingredient_fresh(32, fresh_intervals), false);

    expect(solve_problem_1("example"s), 3ull);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 14ull);
    std::cout << solve_problem_2("first"s) << std::endl;
}
