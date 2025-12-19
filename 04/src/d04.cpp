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

std::vector<bool> process_line(std::string const& line) {
    std::vector<bool> row(line.length() + 2, false);
    for (auto [col, c] : std::views::enumerate(line)) {
        if (c == '@') {
            row[col+1] = true;
        }
    }

    return row;
}

std::generator<std::vector<bool>> read_input(std::string const& filename) {
    std::filesystem::path const base = "./04/input"s;
    auto const filepath = base / filename;

    std::ifstream stream(filepath);
    std::string line;

    while (std::getline(stream, line) && !line.empty()) {
        co_yield process_line(line);
    }
}

unsigned long long count_removed_roll(std::string const& filename, unsigned int nb_tries) {
    std::vector<std::vector<bool>> grid;
    grid.emplace_back();    // Add a top border

    for (auto const& row : read_input(filename)) {
        grid.emplace_back(row);
    }

    size_t const width = grid.back().size();
    grid.front().assign(width, false);
    grid.emplace_back(width, false);

    unsigned long long result = 0;
    for (unsigned int try_no = 0; try_no < nb_tries; ++try_no) {
        std::vector<std::pair<size_t, size_t>> to_remove;

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
                        to_remove.emplace_back(row, col);
                    }
                }
            }
        }

        result += to_remove.size();

        for (auto const& [r, c] : to_remove) {
            grid[r][c] = false;
        }

        if (to_remove.empty()) {
            break;
        }
    }

    return result;
}

unsigned long long solve_problem_1(std::string const& filename) {
    return count_removed_roll(filename, 1);
}

unsigned long long solve_problem_2(std::string const& filename) {
    return count_removed_roll(filename, std::numeric_limits<unsigned int>::max());
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

    expect(solve_problem_2("example"s), 43);
    std::cout << solve_problem_2("first"s) << std::endl;
}
