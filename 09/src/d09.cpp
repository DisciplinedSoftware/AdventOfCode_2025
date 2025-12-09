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

std::vector<std::pair<long long, long long>> parse(std::string const & filename) {
    std::filesystem::path const base = "./09/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    std::vector<std::pair<long long, long long>> points;

    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        long long x, y;
        char dummy;
        line_stream >> x >> dummy >> y;
        points.emplace_back(x, y);
    }

    return points;
}

unsigned long long solve_problem_1(std::vector<std::pair<long long, long long>> const& points) {
    std::vector<unsigned long long> areas;
    for (size_t i = 0; i < points.size(); ++i) {
        auto const [x1, y1] = points[i];
        for (size_t j = i+1; j < points.size(); ++j) {
            auto const [x2, y2] = points[j];
            unsigned long long const area = (std::abs((x2 - x1)) + 1) * (std::abs((y2 - y1)) + 1);
            areas.emplace_back(area);
        }
    }

    return *std::ranges::max_element(areas);
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(parse(filename));
}

bool point_in_polygon(long long px, long long py, std::vector<std::pair<long long, long long>> const& polygon) {
    size_t const n = polygon.size();
    bool inside = false;
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        auto [xi, yi] = polygon[i];
        auto [xj, yj] = polygon[j];
        if (((yi > py) != (yj > py)) &&
            (px < (xj - xi) * (py - yi) / static_cast<double>(yj - yi) + xi)) {
            inside = !inside;
        }
    }
    return inside;
}

unsigned long long solve_problem_2(std::vector<std::pair<long long, long long>> const& points) {
    auto const [min_x, max_x] = std::ranges::minmax_element(points, {}, &std::pair<long long, long long>::first);
    auto const [min_y, max_y] = std::ranges::minmax_element(points, {}, &std::pair<long long, long long>::second);

    // Enfore a contour of non-green and non-red tiles
    std::vector<std::vector<bool>> grid(max_x->first - min_x->first + 1,
                                           std::vector<bool>(max_y->second - min_y->second + 1, false));
    for (size_t i = 1; i < points.size(); ++i) {
        auto begin = points[i-1];
        auto end = points[i];
        begin.first -= min_x->first;
        begin.second -= min_y->second;
        end.first -= min_x->first;
        end.second -= min_y->second;
        for (long long x = std::min(begin.first, end.first); x <= std::max(begin.first, end.first); ++x) {
            for (long long y = std::min(begin.second, end.second); y <= std::max(begin.second, end.second); ++y) {
                grid[x][y] = true;
            }
        }
    }

    auto begin = points.back();
    auto end = points.front();
    begin.first -= min_x->first;
    begin.second -= min_y->second;
    end.first -= min_x->first;
    end.second -= min_y->second;
    for (long long x = std::min(begin.first, end.first); x <= std::max(begin.first, end.first); ++x) {
        for (long long y = std::min(begin.second, end.second); y <= std::max(begin.second, end.second); ++y) {
            grid[x][y] = true;
        }
    }

    // Fill interior using point-in-polygon
    for (long long x = min_x->first; x <= max_x->first; ++x) {
        for (long long y = min_y->second; y <= max_y->second; ++y) {
            auto const dx = x - min_x->first;
            auto const dy = y - min_y->second;
            if (!grid[dx][dy] && point_in_polygon(x, y, points)) {

                grid[dx][dy] = true;
            }
        }
    }

    std::vector<unsigned long long> areas;
    for (size_t i = 0; i < points.size(); ++i) {
        auto [x1, y1] = points[i];
        x1 -= min_x->first;
        y1 -= min_y->second;
        for (size_t j = i+1; j < points.size(); ++j) {
            auto [x2, y2] = points[j];
            x2 -= min_x->first;
            y2 -= min_y->second;
            bool green_of_red = true;
            for (long long x = std::min(x1, x2); green_of_red && x <= std::max(x1, x2); ++x) {
                for (long long y = std::min(y1, y2); green_of_red && y <= std::max(y1, y2); ++y) {
                    green_of_red = grid[x][y];
                }
            }
            if (green_of_red) {
                unsigned long long const area = (std::abs((x2 - x1)) + 1) * (std::abs((y2 - y1)) + 1);
                areas.emplace_back(area);
            }
        }
    }

    return *std::ranges::max_element(areas);
}

unsigned long long solve_problem_2(std::string const& filename) {
    return solve_problem_2(parse(filename));
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
    expect(solve_problem_1("example"s), 50);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 24);
    std::cout << solve_problem_2("first"s) << std::endl;
}
