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
#include <limits>
#include <cstdint>

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
using shape = std::array<std::bitset<shape_size>, shape_size>;

namespace std {
    template <>
    struct hash<shape> {
        std::size_t operator()(const shape& s) const noexcept {
            size_t hash = 0;
            for (size_t i = 0; i < shape_size; ++i) {
                hash = (hash << shape_size) + s[i].to_ullong();
            }
            return hash;
        }
    };
}

constexpr size_t nb_shapes = 6;

unsigned long long solve_problem_1(std::ifstream&& stream) {
    unsigned long long result = 0;

    // Parse shapes
    std::array<std::vector<shape>, nb_shapes> shapes{};
    std::array<size_t, nb_shapes> shape_cell_counts{};

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
            shape_cell_counts[shape_index] += current_shape[i].count();
        }

        std::unordered_set<shape> current_shapes;
        current_shapes.reserve(4*3);

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
        std::array<size_t, nb_shapes> counts;
        auto previous_separator = colon_separator + 2;
        for (size_t s = 0; s < shapes.size(); ++s) {
            auto next_space = line.find(' ', previous_separator);
            if (next_space == std::string::npos) {
                next_space = line.size();
            }
            counts[s] = std::stoul(line.substr(previous_separator, next_space - (previous_separator)));
            previous_separator = next_space + 1;
        }

        // Early rejection by cell count
        size_t const total_required = std::ranges::fold_left(std::views::zip(counts, shape_cell_counts), 0ull, [&](size_t acc, auto s) {
            return acc + std::get<0>(s) * std::get<1>(s);
        });

        if (total_required > width * length) {
            //std::cout << "Impossible by cell-count: " << total_required << " > " << (width * length) << std::endl;
            continue;
        }

        assert(width <= 64);
        std::vector<std::bitset<64>> area_rows(length, 0);

        // Order shape indices by descending cell count (helps prune)
        std::array<size_t, nb_shapes> shape_order{};
        std::ranges::iota(shape_order, 0);
        std::ranges::sort(shape_order, [&shape_cell_counts](size_t a, size_t b) {
                return shape_cell_counts[a] > shape_cell_counts[b];
            });

        auto can_place_shape = [&area_rows, width](shape const& s, size_t bx, size_t by) -> bool {
            return std::ranges::all_of(std::views::enumerate(s), [&area_rows, bx, by](auto const & pr) {
                    auto const [i, row] = pr;
                    size_t const yy = by + i;
                    auto const shifted = (row.to_ullong() << bx);
                    return (area_rows[yy].to_ullong() & shifted) == 0;
                });
        };

        // auto print_area = [&area_rows, length, width]() {
        //     for (size_t j = 0; j < length; ++j) {
        //         for (size_t i = 0; i < width; ++i) {
        //             if (area_rows[j][i]) {
        //                 std::cout << '#';
        //             }
        //             else {
        //                 std::cout << '.';
        //             }
        //         }
        //         std::cout << std::endl;
        //     }
        //     std::cout << "----" << std::endl;
        // };

        auto solve = [&area_rows, &counts, &shapes, &shape_order, length, width, &can_place_shape](this auto&& self) -> bool {

            if (std::ranges::all_of(counts, [](auto c){ return c == 0; })) {
                return true;
            }

            size_t best_shape = std::numeric_limits<size_t>::max();
            size_t best_options = std::numeric_limits<size_t>::max();
            for (size_t shape_id :shape_order) {
                if (counts[shape_id] == 0) {
                    continue;
                }

                size_t const options = [&shapes, &area_rows, length, width, shape_id, &can_place_shape]() {
                    size_t options = 0;
                    for (auto const & s : shapes[shape_id]) {
                        for (size_t by = 0; by <= length - shape_size; ++by) {
                            for (size_t bx = 0; bx <= width - shape_size; ++bx) {
                                if (can_place_shape(s, bx, by)) {
                                    ++options;
                                }
                            }
                        }
                    }
                    return options;
                }();

                if (options == 0) {
                    return false;
                }
                if (options < best_options) {
                    best_options = options;
                    best_shape = shape_id;
                }
            }

            size_t const shape_id = best_shape;
            for (auto const& s : shapes[shape_id]) {
                for (size_t by = 0; by <= length - shape_size; ++by) {
                    for (size_t bx = 0; bx <= width - shape_size; ++bx) {
                        if (!can_place_shape(s, bx, by)) {
                            continue;
                        }

                        for (auto const& [i, row] : std::views::enumerate(s)) {
                            area_rows[by + i] |= (row.to_ullong() << bx);
                        }
                        --counts[shape_id];

                        if (self()) {
                            return true;
                        }

                        ++counts[shape_id];
                        for (auto const& [i, row] : std::views::enumerate(s)) {
                            area_rows[by + i] ^= (row.to_ullong() << bx);
                        }
                    }
                }
            }

            return false;
        };

        // std::cout << "Attempting to fill area " << width << "x" << length << std::endl;
        if (solve()) {
            ++result;
        }
    }

    return result;
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(open(filename));
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
    std::cout << solve_problem_1("first"s) << std::endl;
}
