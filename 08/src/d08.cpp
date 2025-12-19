#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <generator>
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

struct Point3D {
    unsigned long long x;
    unsigned long long y;
    unsigned long long z;
};

namespace std {
template<>
struct tuple_size<Point3D> : integral_constant<std::size_t, 3> {};

template<size_t N>
struct tuple_element<N, Point3D> {
    using type = unsigned long long;
};
}

template<size_t N>
unsigned long long& get(Point3D& p) {
    if constexpr (N == 0) return p.x;
    else if constexpr (N == 1) return p.y;
    else if constexpr (N == 2) return p.z;
    else static_assert(N < 3, "Index out of bounds in std::get<Point3D>");
}
template<size_t N>
unsigned long long const& get(Point3D const& p) {
    if constexpr (N == 0) return p.x;
    else if constexpr (N == 1) return p.y;
    else if constexpr (N == 2) return p.z;
    else static_assert(N < 3, "Index out of bounds in std::get<Point3D>");
}

std::generator<Point3D> read_points(std::string const& filename) {
    std::filesystem::path const base = "./08/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        std::istringstream line_stream(line);
        unsigned long long x, y, z;
        char dummy;
        line_stream >> x >> dummy >> y >> dummy >> z;
        co_yield Point3D{x, y, z};
    }
}

unsigned long long solve_problem_1(std::string const& filename, unsigned int nb_shortest_connections) {
    std::vector<Point3D> const connections = read_points(filename) | std::ranges::to<std::vector>();

    std::vector<std::pair<unsigned long long, std::pair<size_t, size_t>>> distances;
    for (size_t i = 0; i < connections.size(); ++i) {
        auto const& [x1, y1, z1] = connections[i];
        for (size_t j = i+1; j < connections.size(); ++j) {
            auto const& [x2, y2, z2] = connections[j];
            unsigned long long const dx = x2 - x1;
            unsigned long long const dy = y2 - y1;
            unsigned long long const dz = z2 - z1;
            unsigned long long const distance = dx*dx + dy*dy + dz*dz;
            distances.emplace_back(distance, std::make_pair(i, j));
        }
    }

    std::multimap<size_t, size_t> distance_map;
    std::ranges::sort(distances);
    for (unsigned int k = 0; k < nb_shortest_connections && k < distances.size(); ++k) {
        distance_map.emplace(distances[k].second.first, distances[k].second.second);
        distance_map.emplace(distances[k].second.second, distances[k].second.first);
    }

    std::vector<unsigned long long> nb_junction_boxes;
    std::unordered_set<size_t> visited;
    size_t sum = 0;
    for (auto const& [point_index, connected_points] : distance_map) {
        std::queue<size_t> to_visit;
        to_visit.push(point_index);
        auto const [begin, end] = distance_map.equal_range(point_index);
        while (!to_visit.empty()) {
            size_t current = to_visit.front();
            to_visit.pop();
            if (visited.contains(current)) {
                continue;
            }
            visited.emplace(current);
            auto const [conn_begin, conn_end] = distance_map.equal_range(current);
            for (auto it = conn_begin; it != conn_end; ++it) {
                if (!visited.contains(it->second)) {
                    to_visit.push(it->second);
                }
            }
        }
        nb_junction_boxes.emplace_back(visited.size() - sum);
        sum = visited.size();
    }

    std::ranges::sort(nb_junction_boxes, std::greater<>{});
    return std::ranges::fold_left(nb_junction_boxes.begin(), nb_junction_boxes.begin()+3, 1ULL, std::multiplies<>{});
}

unsigned long long solve_problem_2(std::string const& filename) {
    std::vector<Point3D> const connections = read_points(filename) | std::ranges::to<std::vector>();

    std::vector<std::pair<unsigned long long, std::pair<size_t, size_t>>> distances;
    for (size_t i = 0; i < connections.size(); ++i) {
        auto const& [x1, y1, z1] = connections[i];
        for (size_t j = i+1; j < connections.size(); ++j) {
            auto const& [x2, y2, z2] = connections[j];
            unsigned long long const dx = x2 - x1;
            unsigned long long const dy = y2 - y1;
            unsigned long long const dz = z2 - z1;
            unsigned long long const distance = dx*dx + dy*dy + dz*dz;
            distances.emplace_back(distance, std::make_pair(i, j));
        }
    }

    std::pair<Point3D, Point3D> result;
    std::vector<std::unordered_set<size_t>> junctions;
    std::ranges::sort(distances);
    for (auto const& [distance, connection] : distances) {
        auto it_first = std::ranges::find_if(junctions, [&connection](auto const& junction) {
            return junction.contains(connection.first);
        });
        auto it_second = std::ranges::find_if(junctions, [&connection](auto const& junction) {
            return junction.contains(connection.second);
        });
        if (it_first != junctions.end() && it_first == it_second) {
            continue;
        }

        if (it_first != junctions.end() && it_second != junctions.end()) {
            std::unordered_set<size_t>& set_first = *it_first;
            std::unordered_set<size_t>& set_second = *it_second;
            std::merge(set_first.begin(), set_first.end(),
                       set_second.begin(), set_second.end(),
                       std::inserter(set_first, set_first.end()));
            junctions.erase(it_second);
        }
        else if (it_first != junctions.end()) {
            it_first->emplace(connection.second);
        }
        else if (it_second != junctions.end()) {
            it_second->emplace(connection.first);
        }
        else {
            junctions.emplace_back(std::unordered_set<size_t>{connection.first, connection.second});
        }

        auto it_full = std::find_if(junctions.begin(), junctions.end(), [&connections](auto const& junction) {
            return junction.size() == connections.size();
        });
        if (it_full != junctions.end()) {
            result = {connections[connection.first], connections[connection.second]};
            break;
        }
    }

    return result.first.x * result.second.x;
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
    expect(solve_problem_1("example"s, 10), 40);
    std::cout << solve_problem_1("first"s, 1000) << std::endl;

    expect(solve_problem_2("example"s), 25272);
    std::cout << solve_problem_2("first"s) << std::endl;
}
