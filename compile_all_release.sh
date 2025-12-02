#!/bin/bash

# Measure time for each program
for i in {01..25}; do
    echo "Compiling d$i"
    g++ -fdiagnostics-color=always ./$i/src/d$i.cpp -o ./out/Release/d$i -std=c++23 -O3 -DNDEBUG
done
