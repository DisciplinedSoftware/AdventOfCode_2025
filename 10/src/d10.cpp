#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
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
        // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}

        std::vector<unsigned int> joltage_final_state;
        std::vector<std::vector<unsigned int>> buttons;

        size_t i = 1;
        while (i < line.size()) {
            if (line[i] == '{') {
                ++i;
                while (i < line.size() && line[i] != '}') {
                    size_t light_index = 0;
                    while (i < line.size() && std::isdigit(line[i])) {
                        light_index = light_index * 10 + (line[i] - '0');
                        ++i;
                    }
                    joltage_final_state.emplace_back(light_index);
                    if (i < line.size() && line[i] == ',') {
                        ++i;
                    }
                }
                ++i; // skip the closing '}'
                break;
            }
            else if (line[i] == '(') {
                ++i;
                std::vector<unsigned int> button_joltages;
                while (i < line.size() && line[i] != ')') {
                    size_t light_index = 0;
                    while (i < line.size() && std::isdigit(line[i])) {
                        light_index = light_index * 10 + (line[i] - '0');
                        ++i;
                    }
                    button_joltages.emplace_back(light_index);
                    if (i < line.size() && line[i] == ',') {
                        ++i;
                    }
                }
                buttons.emplace_back(button_joltages);
            }

            ++i;
        }

        auto const max_joltage = *std::max_element(joltage_final_state.begin(), joltage_final_state.end());
        auto const bump = max_joltage + 1;
        auto hash_fct = [bump](std::vector<unsigned int> const& vec) {
            size_t seed = 0;
            for(auto const i : vec) {
                seed += seed * bump + i;
            }
            return seed;
        };

        std::unordered_set<std::vector<unsigned int>, decltype(hash_fct)> visited_states(0, hash_fct);
        // Use bfs to find the minimum number of presses to turn on all lights_final_state
        std::queue<std::pair<std::vector<unsigned int>, unsigned long long>> bfs_queue;

        bfs_queue.emplace(std::vector<unsigned int>(joltage_final_state.size(), 0), 0);
        visited_states.emplace(std::vector<unsigned int>(joltage_final_state.size(), 0));
        while (!bfs_queue.empty()) {
            auto [joltage_current_state, nb_buttons_pressed] = bfs_queue.front();
            bfs_queue.pop();

            if (joltage_current_state == joltage_final_state) {
                result += nb_buttons_pressed;
                break;
            }

            for (size_t i = 0; i <buttons.size(); ++i) {
                auto joltage_next_state = joltage_current_state;
                bool valid = true;
                for (auto const j : buttons[i]) {
                    joltage_next_state[j] += 1;
                    if (joltage_next_state[j] > joltage_final_state[j]) {
                        joltage_next_state = joltage_current_state; // invalid state
                        valid = false;
                        break;
                    }
                }

                if (valid && !visited_states.contains(joltage_next_state)) {
                    visited_states.emplace(joltage_next_state);
                    bfs_queue.emplace(joltage_next_state, nb_buttons_pressed + 1);
                }
            }
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
