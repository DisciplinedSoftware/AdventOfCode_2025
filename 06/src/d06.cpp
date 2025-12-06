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

unsigned long long solve_problem_1(std::string const& filename) {
    std::filesystem::path const base = "./06/input"s;
    auto const filepath = base / filename;

    std::ifstream stream(filepath);
    if (stream.fail()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    auto const lineOp = lines.back();
    lines.pop_back();

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

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
