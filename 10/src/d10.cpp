#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
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

#include <z3++.h>


using namespace std::string_literals;

std::ifstream open(std::string const & filename) {
    std::filesystem::path const base = "./10/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    return stream;
}

unsigned long long solve_problem_1(std::ifstream&& stream) {
    unsigned long long result = 0;

    std::string line;
    while (std::getline(stream, line)) {
        // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
        std::bitset<32> lights_final_state;
        std::vector<std::bitset<32>> buttons;

        size_t i = 1;
        while (i < line.size()) {
            if (line[i] == ']') {
                ++i;
                break;
            }
            else if (line[i] == '#') {
                lights_final_state.set(i-1);
            }
            else if (line[i] == '.') {
                // Noting to do
            }
            ++i;
        }

        while (i < line.size()) {
            if (line[i] == '{') {
                break;
            }
            else if (line[i] == '(') {
                ++i;
                std::bitset<32> button_lights;
                while (i < line.size() && line[i] != ')') {
                    size_t light_index = 0;
                    while (i < line.size() && std::isdigit(line[i])) {
                        light_index = light_index * 10 + (line[i] - '0');
                        ++i;
                    }
                    button_lights.set(light_index);
                    if (i < line.size() && line[i] == ',') {
                        ++i;
                    }
                }
                buttons.emplace_back(button_lights);
            }

            ++i;
        }

        std::unordered_set<std::bitset<32>> visited_states;
        // Use bfs to find the minimum number of presses to turn on all lights_final_state
        std::queue<std::pair<std::bitset<32>, unsigned long long>> bfs_queue;

        bfs_queue.emplace(std::bitset<32>{}, 0);
        visited_states.emplace(std::bitset<32>{});
        while (!bfs_queue.empty()) {
            auto [light_current_state, nb_buttons_pressed] = bfs_queue.front();
            bfs_queue.pop();

            if (light_current_state == lights_final_state) {
                result += nb_buttons_pressed;
                break;
            }

            for (size_t i = 0; i <buttons.size(); ++i) {
                auto const light_next_state = light_current_state ^ buttons[i];
                if (!visited_states.contains(light_next_state)) {
                    visited_states.emplace(light_next_state);
                    bfs_queue.emplace(light_next_state, nb_buttons_pressed + 1);
                }
            }
        }
    }

    return result;
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(open(filename));
}

unsigned long long solve_problem_2(std::ifstream&& stream) {
    unsigned long long result = 0;

    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        std::vector<std::vector<int>> buttons;
        std::vector<unsigned long long> target;

        // parse line: collect all '(' groups as buttons and the '{' group as target
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '(') {
                ++i;
                std::vector<int> btn;
                while (i < line.size() && line[i] != ')') {
                    if (std::isdigit(line[i])) {
                        int val = 0;
                        while (i < line.size() && std::isdigit(line[i])) {
                            val = val * 10 + (line[i] - '0');
                            ++i;
                        }
                        btn.push_back(val);
                    } else {
                        ++i;
                    }
                }
                buttons.emplace_back(std::move(btn));
            } else if (line[i] == '{') {
                ++i;
                while (i < line.size() && line[i] != '}') {
                    if (std::isdigit(line[i])) {
                        unsigned long long val = 0ULL;
                        while (i < line.size() && std::isdigit(line[i])) {
                            val = val * 10ULL + static_cast<unsigned long long>(line[i] - '0');
                            ++i;
                        }
                        target.push_back(val);
                    } else {
                        ++i;
                    }
                }
            }
        }

        using namespace z3;
        context ctx;
        optimize opt(ctx);

        unsigned var_count = static_cast<unsigned>(buttons.size());
        std::vector<expr> xs; xs.reserve(var_count);
        for (unsigned v = 0; v < var_count; ++v) {
            xs.emplace_back(ctx.int_const(("x" + std::to_string(v)).c_str()));
            opt.add(xs.back() >= 0);
        }

        // sum upper bound to help the optimizer
        unsigned long long sum_target = 0ULL;
        for (auto t : target) sum_target += t;
        expr sum_expr = ctx.int_val(0);
        for (unsigned v = 0; v < var_count; ++v) sum_expr = sum_expr + xs[v];
        opt.add(sum_expr <= ctx.int_val(static_cast<int64_t>(sum_target)));

        // per-row equality constraints
        for (size_t r = 0; r < target.size(); ++r) {
            expr row_sum = ctx.int_val(0);
            for (unsigned c = 0; c < var_count; ++c) {
                for (int idx : buttons[c]) {
                    if (static_cast<size_t>(idx) == r) {
                        row_sum = row_sum + xs[c];
                        break;
                    }
                }
            }
            opt.add(row_sum == ctx.int_val(static_cast<int64_t>(target[r])));
        }

        opt.minimize(sum_expr);
        auto s = opt.check();
        if (s == z3::sat) {
            model m = opt.get_model();
            unsigned long long total = 0ULL;
            for (unsigned v = 0; v < var_count; ++v) {
                expr val = m.eval(xs[v], true);
                if (val.is_numeral()) {
                    std::string num = Z3_get_numeral_string(ctx, val);
                    long long iv = 0;
                    try { iv = std::stoll(num); } catch (...) { iv = 0; }
                    if (iv < 0) iv = 0;
                    total += static_cast<unsigned long long>(iv);
                }
            }
            result += total;
        } else {
            throw std::runtime_error("Z3 could not find a solution for line: " + line);
        }
    }

    return result;
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
    expect(solve_problem_1("example"s), 7);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 33);
    std::cout << solve_problem_2("first"s) << std::endl;
}
