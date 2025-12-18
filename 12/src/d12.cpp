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

constexpr size_t nb_shapes = 6;

unsigned long long solve_problem_1(std::ifstream&& stream) {
    unsigned long long result = 0;

    // Parse shapes
    std::array<std::vector<shape>, nb_shapes> shapes{};

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

        // Build variant coordinates once
        std::array<std::vector<std::vector<std::pair<size_t, size_t>>>, nb_shapes> variant_coords{};
        std::array<size_t, nb_shapes> shape_cells{};
        for (size_t s = 0; s < shapes.size(); ++s) {
            variant_coords[s].reserve(shapes[s].size());
            for (auto const & var : shapes[s]) {
                std::vector<std::pair<size_t, size_t>> coords;
                for (size_t i = 0; i < shape_size; ++i) {
                    for (size_t j = 0; j < shape_size; ++j) {
                        if (var[i][j]) {
                            coords.emplace_back(i, j);
                        }
                    }
                }
                assert(!coords.empty());

                variant_coords[s].push_back(std::move(coords));
            }

            assert(!variant_coords[s].empty());
            shape_cells[s] = variant_coords[s].front().size();
        }

        // Early rejection by cell count
        size_t const total_required = std::ranges::fold_left(std::views::zip(counts, shape_cells), 0ull, [&](size_t acc, auto s) {
            return acc + std::get<0>(s) * std::get<1>(s);
        });

        if (total_required > width * length) {
            //std::cout << "Impossible by cell-count: " << total_required << " > " << (width * length) << std::endl;
            continue;
        }

        // Order shape indices by descending cell count (helps prune)
        std::vector<size_t> shape_order(shapes.size());
        std::iota(shape_order.begin(), shape_order.end(), 0);
        std::sort(shape_order.begin(), shape_order.end(),
            [&shape_cells](size_t a, size_t b) {
                return shape_cells[a] > shape_cells[b];
            });

        assert(width <= 64);

        using RowMask = uint64_t;
        struct VariantMask {
            std::vector<std::pair<size_t, RowMask>> rows;
            size_t max_rx;
            size_t max_ry;
        };

        std::array<std::vector<VariantMask>, nb_shapes> variant_masks{};
        for (size_t s = 0; s < shapes.size(); ++s) {
            for (auto const & coords : variant_coords[s]) {
                if (coords.empty()) {
                    continue;
                }
                size_t min_ry = std::numeric_limits<size_t>::max();
                size_t min_rx = std::numeric_limits<size_t>::max();
                size_t max_ry = std::numeric_limits<size_t>::min();
                size_t max_rx = std::numeric_limits<size_t>::min();
                for (auto const & c : coords) {
                    min_ry = std::min(min_ry, c.first);
                    min_rx = std::min(min_rx, c.second);
                    max_ry = std::max(max_ry, c.first);
                    max_rx = std::max(max_rx, c.second);
                }
                std::map<size_t, RowMask> m;
                for (auto const & c : coords) {
                    size_t ry = c.first - min_ry;
                    size_t rx = c.second - min_rx;
                    m[ry] |= (RowMask(1) << rx);
                }
                std::vector<std::pair<size_t, RowMask>> rows;
                rows.reserve(m.size());
                for (auto &p : m) {
                    rows.push_back(p);
                }
                VariantMask vm;
                vm.rows = std::move(rows);
                vm.max_rx = max_rx - min_rx;
                vm.max_ry = max_ry - min_ry;
                variant_masks[s].push_back(std::move(vm));
            }
        }

        std::vector<RowMask> area_rows(length, 0);

        auto solve = [&area_rows, &counts, &variant_masks, &shape_order, length, width](this auto&& self) -> bool {
            if (std::ranges::all_of(counts, [](auto c){ return c == 0; })) {
                return true;
            }

            size_t best_shape = std::numeric_limits<size_t>::max();
            size_t best_options = std::numeric_limits<size_t>::max();
            for (size_t shape_id :shape_order) {
                if (counts[shape_id] == 0) {
                    continue;
                }

                size_t const options = [&variant_masks, &area_rows, length, width, shape_id]() {
                    size_t options = 0;
                    for (auto const & vm : variant_masks[shape_id]) {
                        size_t max_ry = vm.max_ry;
                        size_t max_rx = vm.max_rx;
                        for (size_t by = 0; by <= length - 1 - max_ry; ++by) {
                            for (size_t bx = 0; bx <= width - 1 - max_rx; ++bx) {
                                bool const ok = std::ranges::all_of(vm.rows, [&area_rows, bx, by](auto const & pr) {
                                    size_t const yy = by + pr.first;
                                    RowMask const shifted = (pr.second << bx);
                                    return (area_rows[yy] & shifted) == 0;
                                });
                                if (ok) {
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
            for (auto const & vm : variant_masks[shape_id]) {
                size_t const max_ry = vm.max_ry;
                size_t const max_rx = vm.max_rx;
                for (size_t by = 0; by <= length - 1 - max_ry; ++by) {
                    for (size_t bx = 0; bx <= width - 1 - max_rx; ++bx) {
                        bool ok = true;
                        for (auto const & pr : vm.rows) {
                            size_t yy = by + pr.first;
                            RowMask shifted = (pr.second << bx);
                            if ((area_rows[yy] & shifted) != 0) {
                                ok = false;
                                break;
                            }
                        }
                        if (!ok) {
                            continue;
                        }
                        for (auto const & pr : vm.rows) {
                            area_rows[by + pr.first] |= (pr.second << bx);
                        }
                        --counts[shape_id];
                        if (self()) {
                            return true;
                        }
                        ++counts[shape_id];
                        for (auto const & pr : vm.rows) {
                            area_rows[by + pr.first] ^= (pr.second << bx);
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
