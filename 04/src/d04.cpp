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

void process_line(std::string const& line, std::vector<std::vector<bool>>& grid) {
    auto& row = grid.back();
    for (size_t i = 0, col = 1; i < line.length(); ++i, ++col) {
        auto const c = line[i];
        if (c == '@') {
            row[col] = true;
        }
    }
}

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./04/input"s;
    auto const filepath = base / filename;

    std::vector<std::vector<bool>> grid;

    std::ifstream stream(filepath);
    std::string line;
    if (!std::getline(stream, line) || line.empty()) {
        throw std::runtime_error("Unable to read line from file: " + filepath.string());
    }

    size_t width = line.length();

    grid.emplace_back(std::vector<bool>(width + 2, false));
    grid.emplace_back(std::vector<bool>(width + 2, false));

    process_line(line, grid);

    while (std::getline(stream, line) && !line.empty()) {
        grid.emplace_back(std::vector<bool>(width + 2, false));
        process_line(line, grid);
    }

    grid.emplace_back(std::vector<bool>(width + 2, false));
    unsigned long long result = 0;
    for (size_t row = 1; row < grid.size() - 1; ++row) {
        for (size_t col = 1; col < grid[row].size() - 1; ++col) {
            if (grid[row][col]) {
                unsigned int nb_neighbors = 0;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        if (grid[row + dr][col + dc]) {
                            ++nb_neighbors;
                        }
                    }
                }

                if (nb_neighbors < 4) {
                    ++result;
                }
            }
        }
    }

    return result;
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
    expect(solve_problem_1("example"s), 13);
    std::cout << solve_problem_1("first"s) << std::endl;

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
