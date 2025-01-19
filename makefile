# Makefile for managing the project
DOXYGEN_CONFIG = Doxyfile
DOXYGEN_OUTPUT_DIR = docs
PYTHON = python3

CODE_LOC = iteration
APP_NAME = optimizedscheduler
APP_LOC = bin/${APP_NAME}

# Docker variables
DOCKER_IMAGE_NAME = custom_project_image
DOCKER_CONTAINER_NAME = custom_project_container
DOCKERFILE = Dockerfile
RESULTS_JSON = results.json

# Feature 1: Compile and run the latest iteration
.PHONY: compile_run
compile_run:
	@echo "Compiling an iteration locally..."
	@gcc -w -o "${APP_LOC}_7" "${CODE_LOC}_7"/*.c
	@echo "Compilation ended."
	@echo "Run the iteration locally..."
	@${APP_LOC}_7 5
	@echo "Run ended."

# Feature 2: Build Docker image
.PHONY: build_image
build_image:
	@echo "Building Docker image..."
	@./build_docker_images.sh

# Feature 3: Run Docker containers and collect metrics
.PHONY: run_containers
run_containers:
	@echo "Running Docker containers and collecting metrics..."
	@./run_docker_containers.sh

# Feature 4: Run all iterations locally on isolated threads
.PHONY: run_local_analysis
run_local_analysis:
	@echo "Running all iterations locally and collecting metrics..."
	@./run_analysis_local.sh

# Feature 5: Generate documentation
.PHONY: generate_docs
generate_docs:
	@echo "Generating documentation with Doxygen..."
	@doxygen $(DOXYGEN_CONFIG)
	@echo "Documentation generated in $(DOXYGEN_OUTPUT_DIR)/index.html"

# Clean artifacts
.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -rf $(DOXYGEN_OUTPUT_DIR)
	@rm -f *.o *.log
	@echo "Clean complete!"

# Default target: Help
.PHONY: help
help:
	@echo "Available commands:"
	@echo "  make compile_run           - Compile and run the latest iteration"
	@echo "  make build_image           - Build the Docker image"
	@echo "  make run_docker            - Run Docker containers and collect metrics"
	@echo "  make run_local_analysis    - Run all iterations locally and collect metrics"
	@echo "  make generate_docs         - Generate documentation with Doxygen"
	@echo "  make clean                 - Clean up generated files and artifacts"