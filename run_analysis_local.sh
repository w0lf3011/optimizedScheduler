#!/bin/bash

# Iteration number
ITERATIONS=7

# TDP of Processor in use (depending of host hardware)
TDP=95
NUM_CORE=16
TDP_PER_CORE=$(($TDP / $NUM_CORE))

CPU_CORE=0

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

# Loop of X Iterations configured
for i in $(seq 0 $ITERATIONS); do
    ITERATION_DIR="Iteration_${i}"
    APP_NAME="optimizedscheduler${i}"
    APP="bin/${APP_NAME}"
    LOG_FILE="output/${APP_NAME}_localrun_$(date +"%Y%m%d_%H%M%S").log"
    WRAPPER_SCRIPT="bin/wrapper_${APP_NAME}.sh"

    echo "Starting the Local Analysis of '${ITERATION_DIR}' on CPU Core ${CPU_CORE} ...\n"    
    echo "Total TDP: ${TDP} W" >> "${LOG_FILE}"
    echo "Number of Cores: ${NUM_CORE}" >> "${LOG_FILE}"
    echo "TDP per Core: ${TDP_PER_CORE} W" >> "${LOG_FILE}"
    echo "Using CPU Core: ${CPU_CORE}" >> "${LOG_FILE}"

    # Check present of Iteration Folder.
    if [ ! -d "${ITERATION_DIR}" ]; then
        echo "Error: Directory '${ITERATION_DIR}' does not exist. Skipping iteration ${i}."
        continue
    fi

    # Compile all c files found, and manage error.
    gcc -w -o "${APP}" "${ITERATION_DIR}"/*.c
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed for iteration ${i}. Skipping." | tee -a "${LOG_FILE}"
        continue
    fi

    # Take a snapshot of CPU stats and timing before execution of program
    read -a cpu_start < <(read_cpu_stats)
    start_time=$(date +"%Y-%m-%d %H:%M:%S")
    start_timestamp=$(date +%s) # Timestamp en secondes
    echo "Start Time: ${start_time}" >> "${LOG_FILE}"

    # Execute the APP, only with the Core CPU_CORE to measure the Usage of one CPU Core and estimate the consumption.
    taskset -c ${CPU_CORE} "${APP}" 5 &>> "${LOG_FILE}"
    if [ $? -ne 0 ]; then
        echo "Error: Valgrind execution failed for iteration ${i}. Check ${LOG_FILE} for details."
        echo "${?}" &>> "${LOG_FILE}"
        continue
    fi

    # Take a snapshot of CPU stats and timing after execution of program
    read -a cpu_end < <(read_cpu_stats)
    end_time=$(date +"%Y-%m-%d %H:%M:%S")
    end_timestamp=$(date +%s) # Timestamp en secondes
    echo "End Time: ${end_time}" >> "${LOG_FILE}"
    
    # Calculer le temps d'exécution
    elapsed_time=$((end_timestamp - start_timestamp))
    echo "Execution Time: ${elapsed_time} seconds" >> "${LOG_FILE}"

    # Calculate Usage CPU
    read active_diff total_diff < <(calculate_cpu_usage cpu_start cpu_end)

    # Calculate CPU Core in Pourcentage
    cpu_usage_percentage=$(awk "BEGIN {print ($active_diff / $total_diff) * 100}")

    # estimate the consumption of CPU Core dedicated in Watts
    power_consumed=$(awk "BEGIN {print ($TDP_PER_CORE * $cpu_usage_percentage) / 100}")

    echo "Iteration $i : CPU Usage = ${cpu_usage_percentage}% | Consummed Watts = ${power_consumed} W"
    echo "CPU CORE ${CPU_CORE} Usage: ${cpu_usage_percentage}%" >> "${LOG_FILE}"
    echo "Power Consumed: ${power_consumed} W" >> "${LOG_FILE}"

    # Execute Valgrind memory leak analysis on the APP.
    valgrind --tool=memcheck --leak-check=full "${APP}" 5 &>> "${LOG_FILE}"
    if [ $? -ne 0 ]; then
        echo "Error: Valgrind execution failed for iteration ${i}. Check ${LOG_FILE} for details."
        echo "${?}" &>> "${LOG_FILE}"
        continue
    fi

    # Revenir au répertoire précédent
    echo "...Ending the Local Analysis of Iteration ${i}."
done

echo "All Iteration completed."