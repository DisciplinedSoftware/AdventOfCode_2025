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

std::generator<std::string> parse_input(std::string const& filename) {
    std::filesystem::path const base = "./06/input"s;
    auto const filepath = base / filename;

    std::ifstream stream(filepath);
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }
    std::string line;
    while (std::getline(stream, line)) {
        co_yield line;
    }
}

std::pair<std::vector<std::string>, std::string> parse(std::string const& filename) {
    auto lines = parse_input(filename) | std::ranges::to<std::vector<std::string>>();

    auto const lineOp = lines.back();
    lines.pop_back();

    return {lines, lineOp};
}

unsigned long long solve_problem_1(std::string const& filename) {
    auto const [lines, lineOp] = parse(filename);

    std::vector<size_t> positions;
    positions.reserve(lines.size());
    for (auto const& line : lines) {
        positions.emplace_back(line.front() == ' ' ? line.find_first_of("0123456789") : 0);
    }

    size_t posOp = lineOp.front() == ' ' ? lineOp.find_first_of("+*") : 0;

    std::vector<unsigned long long> nums(lines.size(), 0);

    unsigned long long result = 0;
    while (posOp < lineOp.size()) {
        for (size_t i = 0; i < lines.size(); ++i) {
            auto const& line = lines[i];
            auto& pos = positions[i];
            auto end = line.find(' ', pos);
            end = end == std::string::npos ? line.size() : end;
            nums[i] = std::stoull(line.substr(pos, end - pos));
            pos = line.find_first_of("0123456789", end);
        }

        auto endOp = lineOp.find(' ', posOp);
        endOp = endOp == std::string::npos ? lineOp.size() : endOp;
        auto const op = lineOp.substr(posOp, endOp - posOp);
        posOp = lineOp.find_first_of("+*", endOp);

        if (op == "+"s) {
            result += std::ranges::fold_left(nums, 0ull, std::plus<>{});
        }
        else if (op == "*"s) {
            result += std::ranges::fold_left(nums, 1ull, std::multiplies<>{});
        }
    }

    return result;
}

std::generator<unsigned long long> extract_numbers(std::vector<std::string> const& lines, size_t& pos) {
    while (pos < lines[0].size()) {
        auto valid = false;
        auto const number = std::ranges::fold_left(lines, 0ull,
            [&valid, &pos](unsigned long long acc, auto const& line) {
                auto c = line[pos];
                if (c != ' ') {
                    valid = true;
                    return acc * 10 + (c - '0');
                }
                return acc;
            });
        ++pos;

        if (!valid) {
            co_return;
        }

        co_yield number;
    }
}

unsigned long long solve_problem_2(std::string const& filename) {
    auto const [lines, lineOp] = parse(filename);

    unsigned long long result = 0;
    size_t pos = 0;
    while (pos < lineOp.size()) {
        char const op = lineOp[pos];
        switch (op) {
            case '+':
                {
                    auto const value = std::ranges::fold_left(extract_numbers(lines, pos), 0ull, std::plus<>{});
                    result += value;
                }
                break;
            case '*':
                {
                    auto const value = std::ranges::fold_left(extract_numbers(lines, pos), 1ull, std::multiplies<>{});
                    result += value;
                }
                break;
            default:
                throw std::runtime_error("Unexpected operator: " + std::string(1, op));
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
    expect(solve_problem_1("example"s), 4277556ull);
    std::cout << solve_problem_1("first"s) << std::endl;

    expect(solve_problem_2("example"s), 3263827ull);
    std::cout << solve_problem_2("first"s) << std::endl;
}
