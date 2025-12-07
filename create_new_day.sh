#!/bin/bash

# Ask for the day number
read -p "Enter the day number: " day

# Format day number to have leading zero if necessary
day_padded=$(printf "%02d" $day)

# Add a question when the folder or file already exists
if [ -d "./$day_padded" ] || [ -f "./$day_padded/src/d$day_padded.cpp" ]; then
    read -p "The directory or file for day $day_padded already exists. Do you want to overwrite it? (y/n) " choice
    case "$choice" in 
      y|Y ) echo "Overwriting...";;
      n|N ) echo "Aborting."; exit 1;;
      * ) echo "Invalid choice. Aborting."; exit 1;;
    esac
fi

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
    std::filesystem::path const base = "./$day_padded/input"s;
    auto const filepath = base / filename;
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        throw std::runtime_error("Could not open file: "s + filepath.string());
    }

    return stream;
}

unsigned long long solve_problem_1(std::ifstream&& stream) {
    unsigned long long result = 0;

    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        
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