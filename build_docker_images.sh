#!/bin/bash

# Project name
IMAGE_BASE_NAME="pico_task_manager"

# Iteration number
ITERATIONS=7

# work folder where Iteration folder are.
CODE_DIR="./"

# Loop on each # Iterations.
for i in $(seq 0 $ITERATIONS); do
    
    IMAGE_NAME="${IMAGE_BASE_NAME}_${i}"
    echo "Image name $IMAGE_NAME"

    ITERATION_DIR="${CODE_DIR}/iteration_${i}"
    echo "Iteration dir $ITERATION_DIR"
    
    if [ -d "$ITERATION_DIR" ]; then
        echo "Building Docker image for iteration $i..."
        docker build -t "$IMAGE_NAME" "$ITERATION_DIR"
    else
        echo "Directory $ITERATION_DIR does not exist. Skipping iteration $i."
    fi
done

echo "Docker images build complete."
