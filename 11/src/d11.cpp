#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <regex>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std::string_literals;

using id = std::array<char, 3>;

namespace std {
    template <>
    struct hash<id> {
        std::size_t operator()(const id& arr) const noexcept {
            size_t hash = 0;
            for (char c : arr) {
                hash = (hash << 8) + c;
            }
            return hash;
        }
    };
}

std::unordered_multimap<id, id> open(std::string const & filename) {
    std::filesystem::path const base = "./11/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    std::unordered_multimap<id, id> devices;
    std::string line;
    while (std::getline(stream, line)) {
        // iii: out
        id device = {line[0], line[1], line[2]};
        size_t i = 5;
        while (i < line.size()) {
            devices.emplace(device, id{line[i+0], line[i+1], line[i+2]});
            i += 4;
        }
    }

    return devices;
}

unsigned long long solve_problem_1(std::unordered_multimap<id, id>&& devices) {
    id constexpr start = {'y', 'o', 'u'};
    id constexpr finish  = {'o', 'u', 't'};

    auto visit = [&devices, &finish](this auto&& self, id const& current, std::unordered_set<id> const& visited) -> unsigned long long {
        if (current == finish) {
            return 1ULL;
        }

        auto [begin, end] = devices.equal_range(current);
        return std::ranges::fold_left(
            std::ranges::subrange(begin, end),
            0ULL,
            [&self, &visited](unsigned long long acc, auto const& pair) -> unsigned long long {
                if (visited.contains(pair.second)) {
                    return acc;
                }
                auto new_visited = visited;
                new_visited.insert(pair.second);
                return acc + self(pair.second, new_visited);
            }
        );
    };

    std::unordered_set<id> visited;
    visited.insert(start);

    return visit(start, visited);
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(open(filename));
}

unsigned long long solve_problem_2(std::unordered_multimap<id, id>&& devices) {
    id constexpr start = {'s', 'v', 'r'};
    id constexpr finish  = {'o', 'u', 't'};

    id constexpr dac = {'d', 'a', 'c'};
    id constexpr fft = {'f', 'f', 't'};

    auto visit = [&devices, &finish, &dac, &fft](
            this auto&& self,
            id const& current,
            std::bitset<2> const& special_visited,
            std::unordered_set<id> const& visited) -> unsigned long long {
        if (current == finish) {
            return special_visited.all() ? 1ULL : 0ULL;
        }

        auto [begin, end] = devices.equal_range(current);
        return std::ranges::fold_left(
            std::ranges::subrange(begin, end),
            0ULL,
            [&self, &special_visited, &visited, &dac, &fft](
                    unsigned long long acc,
                    auto const& pair) -> unsigned long long {
                if (visited.contains(pair.second)) {
                    return acc;
                }

                auto new_special_visited = special_visited;
                if (pair.second == dac) {
                    new_special_visited.set(0);
                }
                else if (pair.second == fft) {
                    new_special_visited.set(1);
                }

                auto new_visited = visited;
                new_visited.insert(pair.second);
                return acc + self(pair.second, new_special_visited, new_visited);
            }
        );
    };

    std::unordered_set<id> visited;
    visited.insert(start);
    std::bitset<2> special_visited;

    return visit(start, special_visited, visited);
}

unsigned long long solve_problem_2(std::string const& filename) {
    return solve_problem_2(open(filename));
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
    expect(solve_problem_1("example"s), 5);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example_2"s), 2);
    std::cout << solve_problem_2("first"s) << std::endl;
}
