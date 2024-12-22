#!/bin/bash

# Ensure jq is installed
if ! command -v jq &> /dev/null
then
    echo "Error: jq is required but not installed. Install it and try again."
    exit 1
fi

# Input and Output files
JSON_FILE=$1
CSV_FILE=$2

if [ -z "$JSON_FILE" ] || [ -z "$CSV_FILE" ]; then
    echo "Usage: ./convert_json2csv.sh <input.json> <output.csv>"
    exit 1
fi

if [ ! -f "$JSON_FILE" ]; then
    echo "Error: JSON file '$JSON_FILE' not found."
    exit 1
fi

# Write the CSV header
echo "Iteration;Run;Execution Time (ms);CPU Usage (%);Power Consumed (W);Valgrind Errors;Valgrind Allocations;Valgrind Frees;Bytes Allocated" > "$CSV_FILE"

# Extract and process the JSON data
jq -r '.[] | 
    .iteration as $iteration |
    .runs[] |
    [$iteration, .run, .execution_time, .cpu_usage, .power_consumed, .valgrind_errors, .valgrind_allocs, .valgrind_frees, .bytes_allocated] | 
    @csv' "$JSON_FILE" | sed 's/,/;/g' >> "$CSV_FILE"

echo "CSV file generated: $CSV_FILE"