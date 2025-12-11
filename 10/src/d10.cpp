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
        std::vector<int> joltage_final_state;
        std::vector<std::vector<int>> buttons;

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
                std::vector<int> button_joltages;
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
        auto hash_fct = [bump](std::vector<int> const& vec) {
            size_t seed = 0;
            for(auto const i : vec) {
                seed += seed * bump + i;
            }
            return seed;
        };

        std::string target_str(joltage_final_state.size(), '\0');
        for (size_t j = 0; j < joltage_final_state.size(); ++j) {
            target_str[j] = static_cast<char>(joltage_final_state[j]);
        }

        std::unordered_set<std::string> visited_states;
        // Use bfs to find the minimum number of presses to turn on all lights_final_state
        std::queue<std::pair<std::string, int>> bfs_queue;

        std::string joltage_initial_state(joltage_final_state.size(), '\0');
        std::ranges::sort(buttons, std::greater{}, [](auto const& btn){ return btn.size(); });
        {
            int nb_buttons_pressed = 0;
            for (size_t i = 0; i < buttons.size(); ++i) {
                int min_value = std::numeric_limits<int>::max();
                for (auto const light_index : buttons[i]) {
                    min_value = std::min(min_value, static_cast<int>(joltage_final_state[light_index]) - joltage_initial_state[light_index]);
                }
                nb_buttons_pressed += min_value;
                for (auto const light_index : buttons[i]) {
                    joltage_initial_state[light_index] += min_value;
                }
            }

            bfs_queue.emplace(joltage_initial_state, nb_buttons_pressed);
        }

        visited_states.emplace(joltage_initial_state);
        while (!bfs_queue.empty()) {
            auto [joltage_current_state, nb_buttons_pressed] = bfs_queue.front();
            bfs_queue.pop();

            if (joltage_current_state == target_str) {
                result += nb_buttons_pressed;
                break;
            }

            for (size_t i = 0; i < buttons.size(); ++i) {
                std::string joltage_next_state = joltage_current_state;
                bool valid = true;
                for (auto const j : buttons[i]) {
                    ++joltage_next_state[j];
                    if (joltage_next_state[j] > target_str[j]) {
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
            auto [joltage_current_state, nb_buttons_pressed, buttons_pressed] = bfs_queue.front();
            bfs_queue.pop();

            if (joltage_current_state == joltage_final_state) {
                result += nb_buttons_pressed;
                break;
            }

            for (size_t i = 0; i < buttons.size(); ++i) {
                auto next_button_pressed = buttons_pressed;
                next_button_pressed[i] += 1;
                auto joltage_next_state_plus = joltage_current_state;
                bool valid_plus = true;
                for (auto const j : buttons[i]) {
                    joltage_next_state_plus[j] += 1;
                    if (joltage_next_state_plus[j] > joltage_final_state[j]) {
                        valid_plus = false;
                    }
                }

                if (valid_plus && !visited_states.contains(joltage_next_state_plus)) {
                    visited_states.emplace(joltage_next_state_plus);
                    auto const sum = std::ranges::fold_left(next_button_pressed, 0, std::plus{});
                    assert(sum == nb_buttons_pressed + 1);
                    bfs_queue.emplace(joltage_next_state_plus, nb_buttons_pressed + 1, next_button_pressed);
                }
            }

            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons_pressed[i] == 0) {
                    continue;
                }

                auto next_button_pressed = buttons_pressed;
                next_button_pressed[i] -= 1;
                auto joltage_next_state_minus = joltage_current_state;
                bool valid_minus = true;
                for (auto const j : buttons[i]) {
                    joltage_next_state_minus[j] -= 1;
                    if (joltage_next_state_minus[j] < 0) {
                        valid_minus = false;
                    }
                }

                if (valid_minus && !visited_states.contains(joltage_next_state_minus)) {
                    visited_states.emplace(joltage_next_state_minus);
                    auto const sum = std::ranges::fold_left(next_button_pressed, 0, std::plus{});
                    assert(sum == nb_buttons_pressed - 1);
                    bfs_queue.emplace(joltage_next_state_minus, nb_buttons_pressed - 1, next_button_pressed);
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
