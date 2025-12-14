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

    // Build adjacency multimap from the devices for faster traversal
    std::unordered_multimap<id, id> adj;
    for (auto const & p : devices) {
        adj.emplace(p.first, p.second);
    }

    // Detect cycles in the graph. If there are no cycles the number of simple
    // paths between two nodes can be computed by DP/memoization in O(V+E).
    enum class Color { WHITE, GRAY, BLACK };
    std::unordered_map<id, Color> color;
    auto dfs_cycle = [&](this auto&& self, const id& u) -> bool {
        color[u] = Color::GRAY;
        auto [b, e] = adj.equal_range(u);
        for (auto it = b; it != e; ++it) {
            auto const & v = it->second;
            auto itc = color.find(v);
            if (itc == color.end() || itc->second == Color::WHITE) {
                if (self(v)) return true;
            } else if (itc->second == Color::GRAY) {
                return true; // back-edge found
            }
        }
        color[u] = Color::BLACK;
        return false;
    };

    bool cyclic = false;
    for (auto const & kv : adj) {
        auto it = color.find(kv.first);
        if (it == color.end() || it->second == Color::WHITE) {
            if (dfs_cycle(kv.first)) { cyclic = true; break; }
        }
    }

    // DP visit for DAGs: memoize count of paths from node -> finish.
    auto dp_visit = [&adj](auto&& self, id const& current, id const& finish, std::unordered_map<id, unsigned long long>& memoizationMap) -> unsigned long long {
        if (current == finish) return 1ULL;
        auto it = memoizationMap.find(current);
        if (it != memoizationMap.end()) return it->second;
        unsigned long long acc = 0ULL;
        auto [b, e] = adj.equal_range(current);
        for (auto it = b; it != e; ++it) {
            acc += self(self, it->second, finish, memoizationMap);
        }
        memoizationMap.emplace(current, acc);
        return acc;
    };

    if (cyclic) {
        throw std::runtime_error("Graph contains cycles, cannot compute number of path."s);
    }

    std::unordered_map<id, unsigned long long> memoizationMap;
    unsigned long long svr_dac = dp_visit(dp_visit, svr, dac, memoizationMap);
    memoizationMap.clear();
    unsigned long long svr_fft = dp_visit(dp_visit, svr, fft, memoizationMap);
    memoizationMap.clear();
    unsigned long long dac_fft = dp_visit(dp_visit, dac, fft, memoizationMap);
    memoizationMap.clear();
    unsigned long long fft_dac = dp_visit(dp_visit, fft, dac, memoizationMap);
    memoizationMap.clear();
    unsigned long long fft_out = dp_visit(dp_visit, fft, out, memoizationMap);
    memoizationMap.clear();
    unsigned long long dac_out = dp_visit(dp_visit, dac, out, memoizationMap);

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
