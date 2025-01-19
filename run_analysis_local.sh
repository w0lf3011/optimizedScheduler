#!/bin/bash

# Iteration number
ITERATIONS=7
REPETITIONS=1

# TDP of Processor in use (depending of host hardware)
TDP=95
NUM_CORE=16
TDP_PER_CORE=$(($TDP / $NUM_CORE))

CPU_CORE=0

JSON_FILE="output/results_$(date +"%Y%m%d_%H%M").json"
LATEX_FILE="output/results_$(date +"%Y%m%d_%H%M").tex"
CSV_FILE="output/results_$(date +"%Y%m%d_%H%M").csv"

# Read CPU stats of CORE 0 from /proc/stat
read_cpu_stats() {
    awk '/^cpu0 / {print $2, $3, $4, $5, $6, $7, $8}' /proc/stat
}

# Calculate CPU Usage between snapshot of /proc/stat
calculate_cpu_usage() {
    local -n start=$1
    local -n end=$2

    local user_diff=$((end[0] - start[0]))
    local nice_diff=$((end[1] - start[1]))
    local system_diff=$((end[2] - start[2]))
    local idle_diff=$((end[3] - start[3]))
    local iowait_diff=$((end[4] - start[4]))
    local irq_diff=$((end[5] - start[5]))
    local softirq_diff=$((end[6] - start[6]))

    local total_diff=$((user_diff + nice_diff + system_diff + idle_diff + iowait_diff + irq_diff + softirq_diff))
    local active_diff=$((user_diff + nice_diff + system_diff + irq_diff + softirq_diff))

    echo "$active_diff $total_diff"
}

RESULT_JSON="["
RESULT_LATEX="\\begin{tabular}{|c|c|c|c|c|c|c|c|c|}\n\\hline\nIteration & Run & Execution Time & CPU Usage & Power Consumed & Valgrind Errors & Valgrind Allocs & Valgrind Frees & Bytes Allocated \\\\\n\\hline\n"
echo "Iteration;Run;Execution Time;CPU Usage;Power Consumed;Valgrind Errors;Valgrind Allocs;Valgrind Frees;Bytes Allocated" > "$CSV_FILE"


# Loop of X Iterations configured
for iteration in $(seq 6 $ITERATIONS); do
    ITERATION_DIR="Iteration_${iteration}"
    APP_NAME="optimizedscheduler${iteration}"
    APP="bin/${APP_NAME}"
    LOG_FILE="output/${APP_NAME}_localrun_$(date +"%Y%m%d_%H%M%S").log"
    

    echo "Starting the Local Analysis of '${ITERATION_DIR}' on CPU Core ${CPU_CORE} ...\n"    
    echo "Total TDP: ${TDP} W" >> "${LOG_FILE}"
    echo "Number of Cores: ${NUM_CORE}" >> "${LOG_FILE}"
    echo "TDP per Core: ${TDP_PER_CORE} W" >> "${LOG_FILE}"
    echo "Using CPU Core: ${CPU_CORE}" >> "${LOG_FILE}"

    # Check present of Iteration Folder.
    if [ ! -d "${ITERATION_DIR}" ]; then
        echo "Error: Directory '${ITERATION_DIR}' does not exist. Skipping iteration ${iteration}."
        continue
    fi

    # Compile all c files found, and manage error.
    gcc -w -o "${APP}" "${ITERATION_DIR}"/*.c
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed for iteration ${i}. Skipping." | tee -a "${LOG_FILE}"
        continue
    fi

    ITERATION_RESULTS="{\"iteration\": $iteration, \"runs\": ["

    # Variables to calculate averages
    TOTAL_TASKS=0 # Keep it as default
    TOTAL_TIME=0
    TOTAL_CPU=0
    TOTAL_POWER=0
    TOTAL_ERRORS=0
    TOTAL_ALLOCS=0
    TOTAL_FREES=0
    TOTAL_BYTES=0

    # Run repetitions
    for run in $(seq 1 $REPETITIONS); do
        
        # Take a snapshot of CPU stats and timing before execution of program
        read -a cpu_start < <(read_cpu_stats)
        start_time=$(date +%s)
        echo "Start Time: ${start_time}" >> "${LOG_FILE}"
        echo "Iteration $iteration - Run ${run} - ${start_time} ..."

        # Execute the APP, only with the Core CPU_CORE to measure the Usage of one CPU Core and estimate the consumption.
        taskset -c ${CPU_CORE} "${APP}" 5 &>> "${LOG_FILE}"
        if [ $? -ne 0 ]; then
            echo "Error: execution failed for iteration ${iteration}. Check ${LOG_FILE} for details."
            continue
        fi

        # Take a snapshot of CPU stats and timing after execution of program
        read -a cpu_end < <(read_cpu_stats)
        end_time=$(date +%s)
        echo "End Time: ${end_time}" >> "${LOG_FILE}"
        
        # Calculer le temps d'exécution
        elapsed_time=$((end_time - start_time))
        echo "Execution Time: ${elapsed_time} seconds" >> "${LOG_FILE}"

        # Calculate Usage CPU
        read active_diff total_diff < <(calculate_cpu_usage cpu_start cpu_end)
        cpu_usage_percentage=$(awk "BEGIN {print ($active_diff / $total_diff) * 100}")
        power_consumed=$(awk "BEGIN {print ($TDP_PER_CORE * $cpu_usage_percentage) / 100}")

        echo "Iteration $iteration : CPU Usage = ${cpu_usage_percentage}% | Consummed Watts = ${power_consumed} W"
        echo "Iteration $iteration : CPU Usage = ${cpu_usage_percentage}% | Consummed Watts = ${power_consumed} W" >> "${LOG_FILE}"
        echo "Iteration $iteration - Run ${run} - CPU Usage = ${cpu_usage_percentage}% | Consummed Watts = ${power_consumed} W"

        # Analyse Valgrind
        valgrind_output=$(valgrind --tool=memcheck --leak-check=full "${APP}" 5 2>&1)
        #echo $valgrind_output
        valgrind_errors=$(echo "$valgrind_output" | grep "ERROR SUMMARY" | awk '{print $4}')
        #echo "Errors: ${valgrind_errors}"
        valgrind_allocs=$(echo "$valgrind_output" | grep "total heap usage" | awk '{print $5}')
        #echo "Allocs: ${valgrind_allocs}"
        valgrind_frees=$(echo "$valgrind_output" | grep "total heap usage" | awk '{print $7}')
        #echo "Frees: ${valgrind_frees}"
        valgrind_bytes=$(echo "$valgrind_output" | grep "bytes allocated" | awk '{print $9}' | sed 's/,//g')
        #echo "Bytes: ${valgrind_bytes}"

        # Accumuler les résultats
        TOTAL_TIME=$((TOTAL_TIME + elapsed_time))
        TOTAL_CPU=$(awk "BEGIN {print $TOTAL_CPU + $cpu_usage_percentage}")
        TOTAL_POWER=$(awk "BEGIN {print $TOTAL_POWER + $power_consumed}")
        TOTAL_ERRORS=$((TOTAL_ERRORS + valgrind_errors))
        TOTAL_ALLOCS=$((TOTAL_ALLOCS + valgrind_allocs))
        TOTAL_FREES=$((TOTAL_FREES + valgrind_frees))
        TOTAL_BYTES=$((TOTAL_BYTES + valgrind_bytes))

        # Append to JSON and CSV
        ITERATION_RESULTS+="{\"run\": $run, \"execution_time\": $elapsed_time, \"cpu_usage\": $cpu_usage_percentage, \"power_consumed\": $power_consumed, \"valgrind_errors\": $valgrind_errors, \"valgrind_allocs\": $valgrind_allocs, \"valgrind_frees\": $valgrind_frees, \"bytes_allocated\": $valgrind_bytes},"
        echo "$iteration;$run;$elapsed_time;$cpu_usage_percentage;$power_consumed;$valgrind_errors;$valgrind_allocs;$valgrind_frees;$valgrind_bytes" >> "$CSV_FILE"
    done

    # Calculate averages and finalize JSON
    ITERATION_RESULTS="${ITERATION_RESULTS%,}], \"tasks_executed\": $TOTAL_TASKS}"
    RESULT_JSON+="$ITERATION_RESULTS,"

    # Calcul des moyennes
    AVG_TIME=$(awk "BEGIN {print $TOTAL_TIME / $REPETITIONS}")
    AVG_CPU=$(awk "BEGIN {print $TOTAL_CPU / $REPETITIONS}")
    AVG_POWER=$(awk "BEGIN {print $TOTAL_POWER / $REPETITIONS}")
    AVG_BYTES=$(awk "BEGIN {print $TOTAL_BYTES / $REPETITIONS}")

    RESULT_LATEX+="$iteration & Avg & $AVG_TIME & $AVG_CPU & $AVG_POWER & 0 & 0 & 0 & $AVG_BYTES \\\\\n\\hline\n"

    # Revenir au répertoire précédent
    echo "...Ending the Local Analysis of Iteration ${iteration}."
done

# Finalize JSON and LaTeX
RESULT_JSON="${RESULT_JSON%,}]"
RESULT_LATEX+="\\end{tabular}"

# Save outputs
echo "$RESULT_JSON" > "$JSON_FILE"
echo -e "$RESULT_LATEX" > "$LATEX_FILE"

echo "Results saved to ${JSON_FILE}, ${LATEX_FILE}, and ${CSV_FILE}."