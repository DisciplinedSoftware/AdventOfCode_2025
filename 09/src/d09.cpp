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

struct Edge {
    long long x1, y1, x2, y2;
    long long dx, dy;  // For interpolation
    Edge(long long x1, long long y1, long long x2, long long y2)
        : x1(x1), y1(y1), x2(x2), y2(y2), dx(x2 - x1), dy(y2 - y1) {}
};

// Scanline fill function
void scanline_fill(std::vector<std::vector<bool>>& grid, const std::vector<std::pair<long long, long long>>& polygon) {
    if (polygon.size() < 3) return;

    std::vector<Edge> edges;
    for (size_t i = 0; i < polygon.size(); ++i) {
        auto p1 = polygon[i];
        auto p2 = polygon[(i + 1) % polygon.size()];
        if (p1.second != p2.second) {  // Ignore horizontal edges
            if (p1.second > p2.second) std::swap(p1, p2);
            edges.emplace_back(p1.first, p1.second, p2.first, p2.second);
        }
    }

    if (edges.empty()) return;

    // Sort edges by min Y
    std::ranges::sort(edges, {}, &Edge::y1);

    long long min_y = edges.front().y1;
    long long max_y = edges.back().y2;

    std::vector<Edge> active_edges;
    size_t edge_idx = 0;

    for (long long y = min_y; y <= max_y; ++y) {
        // Add edges starting at this Y
        while (edge_idx < edges.size() && edges[edge_idx].y1 == y) {
            active_edges.push_back(edges[edge_idx++]);
        }

        // Remove edges ending at this Y
        active_edges.erase(std::remove_if(active_edges.begin(), active_edges.end(),
            [y](const Edge& e) { return e.y2 == y; }), active_edges.end());

        // Calculate X intersections
        std::vector<long long> intersections;
        for (auto const& e : active_edges) {
            if (e.dy == 0) continue;  // Avoid division by zero
            double t = static_cast<double>(y - e.y1) / e.dy;
            long long x = e.x1 + static_cast<long long>(t * e.dx);
            intersections.push_back(x);
        }

        // Sort and fill between pairs
        std::ranges::sort(intersections);
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            long long x_start = intersections[i];
            long long x_end = intersections[i + 1];
            for (long long x = x_start; x <= x_end; ++x) {
                if (x >= 0 && x < static_cast<long long>(grid.size()) &&
                    y >= 0 && y < static_cast<long long>(grid[0].size())) {
                    grid[x][y] = true;
                }
            }
        }
    }
}

unsigned long long solve_problem_2(std::vector<std::pair<long long, long long>> const& points) {
    auto const [min_x, max_x] = std::ranges::minmax_element(points, {}, &std::pair<long long, long long>::first);
    auto const [min_y, max_y] = std::ranges::minmax_element(points, {}, &std::pair<long long, long long>::second);

    // Enforce a contour of non-green and non-red tiles
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

    auto const adj_points = points | std::views::transform([&min_x, &min_y](auto p) {
        return std::make_pair(p.first - min_x->first, p.second - min_y->second);
    }) | std::ranges::to<std::vector<std::pair<long long, long long>>>();

    // Fill using scanline
    scanline_fill(grid, adj_points);

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
