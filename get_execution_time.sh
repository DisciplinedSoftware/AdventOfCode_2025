#!/bin/bash

# Number of runs per program
RUNS=5

# Output file
OUTPUT_FILE="execution_times.txt"

# Create an array to store results
declare -a results

# Start logging to both file and terminal
exec > >(tee -a "$OUTPUT_FILE") 2>&1

# Print header with date
echo "Execution time measurements - $(date)"
echo "Running each program $RUNS times"
echo "----------------------------------------"

# Measure time for each program
for i in {01..12}; do
    # Skip number 14 as it needs to be executed manually
    if [ "$i" = "14" ]; then
        continue
    fi

    program="./build/Release/d$i"
    total_time=0
    
    echo "Running $program $RUNS times..."
    
    # Run the program multiple times
    for ((j=1; j<=RUNS; j++)); do
        # Run the program and capture real time
        output=$( { time $program; } 2>&1 )
        
        # Extract the real time and convert to seconds
        real_time=$(echo "$output" | grep "^real" | awk '{print $2}' | sed 's/m/ * 60 + /;s/s//;s/$//')
        seconds=$(echo "$real_time" | bc -l)
        
        total_time=$(echo "$total_time + $seconds" | bc -l)
        echo "Run $j: $seconds seconds"
    done
    
    # Calculate average
    average=$(echo "scale=3; $total_time / $RUNS" | bc -l)
    
    # Store the result with program name
    results+=("$average $program")
    
    echo "Average time for $(basename $program): $average seconds"
    echo "----------------------------------------"
done

# Sort results numerically in reverse order (slowest first) and format output
printf "\nFinal Results - Average Execution Times (sorted from slowest to fastest):\n\n"
printf "%s\n" "${results[@]}" | sort -rn | while read -r time program; do
    printf "%-20s: %.3f seconds (average of %d runs)\n" "$(basename $program)" "$time" "$RUNS"
done

echo -e "\nResults have been saved to $OUTPUT_FILE"