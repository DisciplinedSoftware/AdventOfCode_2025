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

    auto visit = [&devices](this auto&& self, id const& current, id const& finish, std::unordered_set<id>& visited) -> unsigned long long {
        if (current == finish) {
            return 1ULL;
        }

        auto [begin, end] = devices.equal_range(current);
        return std::ranges::fold_left(
            std::ranges::subrange(begin, end),
            0ULL,
            [&self, &finish, &visited](unsigned long long acc, auto const& pair) -> unsigned long long {
                if (visited.contains(pair.second)) {
                    return acc;
                }
                visited.emplace(pair.second);
                auto result = acc + self(pair.second, finish, visited);
                visited.erase(pair.second);
                return result;
            }
        );
    };

    std::unordered_set<id> visited;
    visited.emplace(start);

    return visit(start, finish, visited);
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(open(filename));
}

unsigned long long solve_problem_2(std::unordered_multimap<id, id>&& devices) {
    // The problem is to find all paths from:
    // - svr to dac (1)
    // - svr to fft (2)
    // - dac to fft (3)
    // - fft to dac (4)
    // - fft to out (5)
    // - dac to out (6)

    // Then multiply the number of paths
    // 1*3*5 + 2*4*6

    id constexpr svr = {'s', 'v', 'r'};
    id constexpr out  = {'o', 'u', 't'};

    id constexpr dac = {'d', 'a', 'c'};
    id constexpr fft = {'f', 'f', 't'};

    auto visit = [&devices](this auto&& self,
                            id const& current,
                            id const& finish,
                            std::unordered_set<id>& visited,
                            std::unordered_map<id, unsigned long long>& memoize) -> unsigned long long {
        if (current == finish) {
            unsigned long long result = 1ULL;
            memoize.emplace(current, result);
            return result;
        }

        auto [begin, end] = devices.equal_range(current);
        return std::ranges::fold_left(
            std::ranges::subrange(begin, end),
            0ULL,
            [&self, finish, &visited, &memoize](unsigned long long acc, auto const& pair) -> unsigned long long {
                if (visited.contains(pair.second)) {
                    return acc;
                }
                visited.emplace(pair.second);
                auto result = acc + self(pair.second, finish, visited, memoize);
                visited.erase(pair.second);
                return result;
            }
        );
    };

    std::unordered_map<id, unsigned long long> memoize;
    std::unordered_set<id> visited;
    visited.insert(svr);
    auto svr_dac = visit(svr, dac, visited, memoize);
    visited.clear();
    memoize.clear();

    visited.insert(svr);
    auto svr_fft = visit(svr, fft, visited, memoize);
    visited.clear();
    memoize.clear();

    visited.insert(dac);
    auto dac_fft = visit(dac, fft, visited, memoize);
    visited.clear();
    memoize.clear();

    visited.insert(fft);
    auto fft_dac = visit(fft, dac, visited, memoize);
    visited.clear();
    memoize.clear();

    visited.insert(fft);
    auto fft_out = visit(fft, out, visited, memoize);
    visited.clear();
    memoize.clear();

    visited.insert(dac);
    auto dac_out = visit(dac, out, visited, memoize);

    return svr_dac * dac_fft * fft_out + svr_fft * fft_dac * dac_out;
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
