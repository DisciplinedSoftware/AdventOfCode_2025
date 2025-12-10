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
    long long x1;
    long long y1;
    long long x2;
    long long y2;
};

bool point_in_polygon(long long px, long long py, std::vector<std::pair<long long, long long>> const& polygon) {
    size_t n = polygon.size();
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

bool point_on_border(long long px, long long py,
                     std::vector<Edge> const& vertical_edges,
                     std::vector<Edge> const& horizontal_edges) {
    for (const auto& edge : vertical_edges) {
        if (px == edge.x1 && edge.y1 <= py && py <= edge.y2) {
            return true;
        }
    }
    for (const auto& edge : horizontal_edges) {
        if (py == edge.y1 && edge.x1 <= px && px <= edge.x2) {
            return true;
        }
    }

    return false;
}

bool edges_intersect(long long r_x1, long long r_y1, long long r_x2, long long r_y2,  // Rectangle edge
                     long long p_x1, long long p_y1, long long p_x2, long long p_y2) {  // Polygon edge
    // Assume axis-aligned: one horizontal, one vertical
    if (r_y1 == r_y2 && p_x1 == p_x2) {  // Rect horizontal, poly vertical
        return (p_x1 >= std::min(r_x1, r_x2) && p_x1 <= std::max(r_x1, r_x2)) &&
               (r_y1 >= std::min(p_y1, p_y2) && r_y1 <= std::max(p_y1, p_y2));
    } else if (r_x1 == r_x2 && p_y1 == p_y2) {  // Rect vertical, poly horizontal
        return (p_y1 >= std::min(r_y1, r_y2) && p_y1 <= std::max(r_y1, r_y2)) &&
               (r_x1 >= std::min(p_x1, p_x2) && r_x1 <= std::max(p_x1, p_x2));
    }

    return false;  // Same orientation or diagonal (no intersect)
}

bool rectangle_inside_polygon(long long min_x, long long min_y,
                              long long max_x, long long max_y,
                              std::vector<Edge> const& vertical_edges,
                              std::vector<Edge> const& horizontal_edges,
                              std::vector<std::pair<long long, long long>> const& points) {
    for (const auto& edge : vertical_edges) {
        if (min_x < edge.x1 && edge.x1 < max_x &&
            (edge.y1 < min_y && min_y < edge.y2 || edge.y1 < max_y && max_y < edge.y2)) {
            return false;
        }
    }
    for (const auto& edge : horizontal_edges) {
        if (min_y < edge.y1 && edge.y1 < max_y &&
            (edge.x1 < min_x && min_x < edge.x2 || edge.x1 < max_x && max_x < edge.x2)) {
            return false;
        }
    }

    bool const inside = std::ranges::all_of(
        std::vector<std::pair<long long, long long>>{
            {min_x, min_y},
            {min_x, max_y},
            {max_x, min_y},
            {max_x, max_y}
        },
        [&points, &vertical_edges, &horizontal_edges](auto const& point) {
            return point_in_polygon(point.first, point.second, points) || 
                   point_on_border(point.first, point.second, vertical_edges, horizontal_edges);
        }
    );

    return inside;
}

unsigned long long solve_problem_2(std::vector<std::pair<long long, long long>> const& points) {
    std::vector<Edge> vertical_edges, horizontal_edges;
    for (size_t i = 1; i < points.size(); ++i) {
        auto const [x1, y1] = points[i-1];
        auto const [x2, y2] = points[i];
        if (x1 == x2) {
            vertical_edges.push_back(Edge{std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2)});
        } else /*if (y1 == y2)*/ {
            horizontal_edges.push_back(Edge{std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2)});
        }
    }

    {
        auto const [x1, y1] = points.back();
        auto const [x2, y2] = points.front();
        if (x1 == x2) {
            vertical_edges.push_back(Edge{x1, y1, x2, y2});
        } else /*if (y1 == y2)*/ {
            horizontal_edges.push_back(Edge{x1, y1, x2, y2});
        }
    }

    std::vector<unsigned long long> areas;
    for (size_t i = 0; i < points.size(); ++i) {
        auto const [x1, y1] = points[i];
        for (size_t j = i + 1; j < points.size(); ++j) {
            auto const [x2, y2] = points[j];
            long long min_x = std::min(x1, x2);
            long long max_x = std::max(x1, x2);
            long long min_y = std::min(y1, y2);
            long long max_y = std::max(y1, y2);

            if (rectangle_inside_polygon(min_x, min_y, max_x, max_y, vertical_edges, horizontal_edges, points)) {
                unsigned long long area = (max_x - min_x + 1ULL) * (max_y - min_y + 1ULL);
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
