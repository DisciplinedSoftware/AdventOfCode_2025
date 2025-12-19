#!/bin/bash

# Measure time for each program
for i in {01..12}; do
    echo "Compiling d$i"
    g++ -fdiagnostics-color=always ./$i/src/d$i.cpp -o ./build/Release/d$i -std=c++23 -O3 -DNDEBUG -lz3
done
