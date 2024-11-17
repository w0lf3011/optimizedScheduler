#!/bin/bash

# Based image on Docker
IMAGE_BASE_NAME="pico_task_manager"

# Duration of Execution of Iterations
# 10 secondes, 60 secondes, 36 minutes 
# DURATIONS=(10 60 2160)
DURATIONS=(10 60)

# Iteration Number
ITERATIONS=7

# Location of Python script for Monitoring
MONITOR_SCRIPT="monitor_metrics.py"

# Execute in sequence based on Duration configured Iterations configured.
for i in $(seq 0 $ITERATIONS); do
    IMAGE_NAME="${IMAGE_BASE_NAME}_${i}"

    for duration in "${DURATIONS[@]}"; do
        echo "Running Docker container for image $IMAGE_NAME for $duration seconds..."
        
        # Run contener in deamon mode
        CONTAINER_ID=$(docker run -d "$IMAGE_NAME")

        # Run Monitoring Script
        python3 "$MONITOR_SCRIPT" "$CONTAINER_ID" "$duration" "metrics_${IMAGE_NAME}_${duration}.log"

        # Stop container
        docker stop "$CONTAINER_ID"
    done
done

echo "Docker containers run complete with metrics recorded."
