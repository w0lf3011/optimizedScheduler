#!/bin/bash

# Project name
IMAGE_BASE_NAME="optimizedscheduler"

# Output folder after compilation
OUTPUT_DIR="bin"

# Iteration number
ITERATIONS=7

# Work folder where Iteration folders are.
CODE_DIR="."
OUTPUT_DIR="${CODE_DIR}/${OUTPUT_DIR}"

# TDP fixe du processeur
TDP=95

# Fonction pour lire les statistiques CPU depuis /proc/stat
read_cpu_stats() {
    awk '/^cpu / {print $2, $3, $4, $5, $6, $7, $8}' /proc/stat
}

# Fonction pour calculer la différence entre deux séries de valeurs CPU
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

# Boucle sur chaque itération
for i in $(seq 0 $ITERATIONS); do
    ITERATION_DIR="Iteration_${i}"
    APP_NAME="${IMAGE_BASE_NAME}${i}"
    APP="${OUTPUT_DIR}/${APP_NAME}"

    # Compiler le code du main.c du dossier courant
    gcc -w -o "${APP}" "${ITERATION_DIR}"/*.c

    # Changer de répertoire vers le dossier OUTPUT_DIR
    cd "${OUTPUT_DIR}"

    # Lire les statistiques CPU avant l'exécution
    read -a cpu_start < <(read_cpu_stats)

    # Exécuter l'application et surveiller avec valgrind
    valgrind --tool=callgrind "${APP_NAME}"

    # Lire les statistiques CPU après l'exécution
    read -a cpu_end < <(read_cpu_stats)

    # Calculer l'utilisation du CPU
    read active_diff total_diff < <(calculate_cpu_usage cpu_start cpu_end)

    # Calculer l'utilisation Adu CPU en pourcentage
    cpu_usage_percentage=$(awk "BEGIN {print ($active_diff / $total_diff) * 100}")

    # Calculer la puissance consommée en watts
    power_consumed=$(awk "BEGIN {print ($TDP * $cpu_usage_percentage) / 100}")

    echo "Iteration $i : CPU Usage = ${cpu_usage_percentage}% | Consummed Watts = ${power_consumed} W"

    # Revenir au répertoire précédent
    cd ".."
done

echo "All Iteration"