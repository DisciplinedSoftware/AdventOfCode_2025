#!/bin/bash

# Ask for the day number
read -p "Enter the day number: " day

# Format day number to have leading zero if necessary
day_padded=$(printf "%02d" $day)

# Create the directory structure
mkdir -p "./$day_padded/src"
mkdir -p "./$day_padded/input"

cat > "./$day_padded/input/example" << EOL
EOL
cat > "./$day_padded/input/first" << EOL
EOL

# Create the cpp file with basic template
cat > "./$day_padded/src/d$day_padded.cpp" << EOL
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

auto parse(std::string const & filename) {
    std::filesystem::path const base = "./$day_padded/input"s;
    auto const filepath = base / filename;

     data;

    std::ifstream stream(filepath);
    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        
    }

    return data;
}


unsigned long long solve_problem_1(auto const & data) {
    return 0;
}

unsigned long long solve_problem_1(std::string const& filename) {
    return solve_problem_1(parse(filename));
}

unsigned long long solve_problem_2(auto const & data) {
    return 0;
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
    expect(solve_problem_1("example"s), );
    // std::cout << solve_problem_1("first"s) << std::endl;

    // expect(solve_problem_2("example"s), );
    // std::cout << solve_problem_2("first"s) << std::endl;
}
EOL

# Make the script executable
echo "Created:"
echo "- Directory: ./$day_padded/src"
echo "- File:      ./$day_padded/src/d$day_padded.cpp"
echo "- Directory: ./$day_padded/input"