#!/bin/bash

# Based image on Docker
IMAGE_BASE_NAME="optimizedscheduler"

# Duration of Execution of Iterations
# 10 secondes, 60 secondes, 36 minutes 
# DURATIONS=(10 60 2160)
DURATIONS=(10 60 2160)

# Iteration Number
ITERATIONS=7

# Location of Python script for Monitoring
MONITOR_SCRIPT="monitor_metrics.py"

# Execute in sequence based on Duration configured Iterations configured.
for i in $(seq 0 $ITERATIONS); do
    IMAGE_NAME="${IMAGE_BASE_NAME}${i}"
    CONTAINER_NAME="${IMAGE_NAME}_container"

    for duration in "${DURATIONS[@]}"; do
        echo "Running Docker container $CONTAINER_NAME for image $IMAGE_NAME for $duration seconds..."
        

        # Check if the container already exist.
        if [ "$(docker ps -aq -f name=^${CONTAINER_NAME}$)" ]; then
            echo "The container $CONTAINER_NAME already exists."

            # Kill the container if it's running
            if [ "$(docker ps -q -f name=^${CONTAINER_NAME}$)" ]; then
                echo "The container $CONTAINER_NAME is currently running. Shutting down in progress ..."
                docker stop $CONTAINER_NAME
            fi

            # Delete the container
            echo "Deleting of the container $CONTAINER_NAME..."
            docker rm $CONTAINER_NAME
        fi

        # Create container in detach mode
        # Limite usage of Host ressources: --memory=512m --cpus=0.5 --cpuset-cpus="0"
        CONTAINER_ID=$(docker run --rm --memory="256m" --cpus="1" --name "${IMAGE_NAME}_container" -dit "$IMAGE_NAME")

        # Run Monitoring Script
        python3 "$MONITOR_SCRIPT" "$CONTAINER_ID" "$duration" "output/metrics_${IMAGE_NAME}_${duration}.log"

        # Stop container
        docker stop "$CONTAINER_ID"
    done
done

echo "Docker containers run complete with metrics recorded."
