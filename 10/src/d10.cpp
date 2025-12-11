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
        std::vector<bool> lights_final_state;
        std::vector<std::vector<size_t>> buttons;

        size_t i = 1;
        while (i < line.size()) {
            if (line[i] == ']') {
                ++i;
                break;
            }
            else if (line[i] == '#') {
                lights_final_state.emplace_back(true);
            }
            else if (line[i] == '.') {
                lights_final_state.emplace_back(false);
            }
            ++i;
        }

        while (i < line.size()) {
            if (line[i] == '{') {
                break;
            }
            else if (line[i] == '(') {
                ++i;
                std::vector<size_t> button_lights;
                while (i < line.size() && line[i] != ')') {
                    size_t light_index = 0;
                    while (i < line.size() && std::isdigit(line[i])) {
                        light_index = light_index * 10 + (line[i] - '0');
                        ++i;
                    }
                    button_lights.emplace_back(light_index);
                    if (i < line.size() && line[i] == ',') {
                        ++i;
                    }
                }
                buttons.emplace_back(button_lights);
            }

            ++i;
        }

        // Use bfs to find the minimum number of presses to turn on all lights_final_state
        std::queue<std::tuple<size_t, std::vector<bool>, std::vector<size_t>>> bfs_queue;
        auto enqueue_buttons = [&](std::vector<bool>const & light_current_state, std::vector<size_t>const & buttons_path) {
            for (size_t i = 0; i <buttons.size(); ++i) {
                bfs_queue.emplace(i, light_current_state, buttons_path);
            }
        };

        enqueue_buttons(std::vector<bool>(lights_final_state.size(), false), {});
        while (true) {
            auto [button_index, light_current_state, buttons_path] = bfs_queue.front();
            bfs_queue.pop();

            for (auto light_index : buttons[button_index]) {
                light_current_state[light_index] = !light_current_state[light_index];
            }
            buttons_path.emplace_back(button_index);

            if (light_current_state == lights_final_state) {
                result += buttons_path.size();
                break;
            }

            enqueue_buttons(light_current_state, buttons_path);
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
        std::istringstream line_stream(line);
        
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

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
