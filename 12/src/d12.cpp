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

std::ifstream open(std::string const & filename) {
    std::filesystem::path const base = "./12/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    return stream;
}

size_t constexpr shape_size = 3;
using shape = std::array<std::array<bool, shape_size>, shape_size>;

namespace std {
    template <>
    struct hash<shape> {
        std::size_t operator()(const shape& s) const noexcept {
            size_t hash = 0;
            for (size_t i = 0; i < shape_size; ++i) {
                for (size_t j = 0; j < shape_size; ++j) {
                    hash = (hash << 1) + static_cast<size_t>(s[i][j]);
                }
            }
            return hash;
        }
    };
}

unsigned long long solve_problem_1(std::ifstream&& stream) {
    unsigned long long result = 0;

    // Parse shapes
    std::array<std::vector<shape>, 6> shapes{};

    for (size_t shape_index = 0; shape_index < shapes.size(); ++shape_index) {
        std::string line;
        if (!std::getline(stream, line)) {
            throw std::runtime_error("Unexpected end of file when reading shape "s + std::to_string(shape_index));
        }

        assert(shape_index == std::stoul(line.substr(0, line.find(':'))));

        shape current_shape{};
        for (size_t i = 0; i < shape_size; ++i) {
            std::getline(stream, line);
            for (size_t j = 0; j < shape_size; ++j) {
                current_shape[i][j] = (line[j] == '#');
            }
        }

        std::unordered_set<shape> current_shapes;
        current_shapes.reserve(4*4*4);

        // Generate rotations and mirrorings
        auto add_and_rotate_shape = [&current_shapes](shape const& s) {
            shape s_copy = s;
            for (size_t r = 0; r < 4; ++r) {
                current_shapes.insert(s_copy);
                shape rotated_shape{};
                for (size_t i = 0; i < shape_size; ++i) {
                    for (size_t j = 0; j < shape_size; ++j) {
                        rotated_shape[j][shape_size - 1 - i] = s_copy[i][j];
                    }
                }
                std::swap(rotated_shape, s_copy);
            }
        };

        add_and_rotate_shape(current_shape);
        // Vertical mirror
        shape mirrored_shape{};
        for (size_t i = 0; i < shape_size; ++i) {
            for (size_t j = 0; j < shape_size; ++j) {
                mirrored_shape[i][shape_size - 1 - j] = current_shape[i][j];
            }
        }
        add_and_rotate_shape(mirrored_shape);
        // Horizontal mirror
        mirrored_shape = {};
        for (size_t i = 0; i < shape_size; ++i) {
            for (size_t j = 0; j < shape_size; ++j) {
                mirrored_shape[shape_size - 1 - i][j] = current_shape[i][j];
            }
        }
        add_and_rotate_shape(mirrored_shape);

        shapes[shape_index].assign(std::make_move_iterator(current_shapes.begin()), std::make_move_iterator(current_shapes.end()));

        std::getline(stream, line); // consume empty line
    }

    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        // Parse areas to fill
        auto const x_separator = line.find('x');
        size_t const width = std::stoul(line.substr(0, x_separator));
        auto const colon_separator = line.find(':', x_separator);
        size_t const length = std::stoul(line.substr(x_separator + 1, colon_separator - x_separator - 1 ));
        std::array<size_t, 6> counts;
        auto previous_separator = colon_separator + 2;
        for (size_t s = 0; s < shapes.size(); ++s) {
            auto next_space = line.find(' ', previous_separator);
            if (next_space == std::string::npos) {
                next_space = line.size();
            }
            counts[s] = std::stoul(line.substr(previous_separator, next_space - (previous_separator)));
            previous_separator = next_space + 1;
        }

        // The goal is to check if all shapes can be fitted into the area
        // of size width x length given the counts available for each shape.
        // A backtracking approach should be sufficient given the constraints.
        // A shape occupies all cells that are true in its 3x3 grid.
        std::vector<std::vector<bool>> area(length, std::vector<bool>(width, false));
        auto backtracking = [&shapes, &counts, &area, &result] (this auto&& self) -> bool {
            // for (size_t j = 0; j < area.size(); ++j) {
            //     for (size_t i = 0; i < area[0].size(); ++i) {
            //         if (area[j][i]) {
            //             std::cout << '#';
            //         }
            //         else {
            //             std::cout << '.';
            //         }
            //     }
            //     std::cout << std::endl;
            // }
            // std::cout << "----" << std::endl;


            if (std::ranges::all_of(counts, [](auto count) { return count == 0; })) {
                // All shapes placed
                return true;
            }

            // Try to place each shape at the current position
            for (size_t shape_index = 0; shape_index < shapes.size(); ++shape_index) {
                if (counts[shape_index] == 0) {
                    continue; // No more shapes of this type available
                }

                auto place_shape = [&shapes, &counts, &area, &result, &self] (size_t shape_index, size_t x, size_t y) -> bool {
                    for (shape const& s : shapes[shape_index]) {
                        // Check if shape can be placed
                        auto can_place = [&area](shape const& s, size_t x, size_t y) -> bool {
                            for (size_t i = 0; i < shape_size; ++i) {
                                for (size_t j = 0; j < shape_size; ++j) {
                                    if (s[i][j]) {
                                        size_t ay = y + i;
                                        size_t ax = x + j;
                                        if (ay >= area.size() || ax >= area[0].size() || area[ay][ax]) {
                                            return false;
                                        }
                                    }
                                }
                            }
                            return true;
                        };

                        if (can_place(s, x, y)) {
                            auto set_shape = [&area](shape const& s, size_t x, size_t y, bool value) {
                                for (size_t i = 0; i < shape_size; ++i) {
                                    for (size_t j = 0; j < shape_size; ++j) {
                                        if (s[i][j]) {
                                            area[y + i][x + j] = value;
                                        }
                                    }
                                }
                            };

                            set_shape(s, x, y, true);
                            --counts[shape_index];

                            // Recurse
                            if (self()) {
                                return true;
                            }

                            set_shape(s, x, y, false);
                            ++counts[shape_index];
                        }
                    }

                    return false;
                };

                for (size_t j = 0; j < area.size(); ++j) {
                    for (size_t i = 0; i < area[0].size(); ++i) {
                        if( place_shape(shape_index, i, j) ) {
                            return true;
                        }
                    }
                }
            }

            return false;
        };

        std::cout << "Attempting to fill area " << width << "x" << length << std::endl;

        if (backtracking()) {
            ++result;
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
    expect(solve_problem_1("example"s), 2);
    // std::cout << solve_problem_1("first"s) << std::endl;

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
