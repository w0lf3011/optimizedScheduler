# Makefile for managing the project
DOXYGEN_CONFIG = Doxyfile
DOXYGEN_OUTPUT_DIR = docs

CODE_LOC = Iteration
APP_NAME = optimizedscheduler
APP_LOC = bin/${APP_NAME}
LATEST_ITERATION = 8

# Docker variables
DOCKER_IMAGE_NAME = custom_project_image
DOCKER_CONTAINER_NAME = custom_project_container
DOCKERFILE = Dockerfile
RESULTS_JSON = results.json

.PHONY: compile_run
compile_run:
	@echo "Compiling Iteration_$(LATEST_ITERATION) locally..."
	@gcc -Wall -Wextra -pedantic -o "${APP_LOC}_$(LATEST_ITERATION)" "${CODE_LOC}_$(LATEST_ITERATION)"/*.c
	@echo "Compilation ended."
	@echo "Running the iteration locally..."
	@${APP_LOC}_$(LATEST_ITERATION) 1
	@echo "Run ended."

.PHONY: build_image
build_image:
	@echo "Building Docker image..."
	@./build_docker_images.sh

.PHONY: run_containers
run_containers:
	@echo "Running Docker containers and collecting metrics..."
	@./run_docker_containers.sh

.PHONY: run_local_analysis
run_local_analysis:
	@echo "Running all iterations locally and collecting metrics..."
	@./run_analysis_local.sh

.PHONY: generate_docs
generate_docs:
	@echo "Generating documentation with Doxygen..."
	@doxygen $(DOXYGEN_CONFIG)
	@echo "Documentation generated in $(DOXYGEN_OUTPUT_DIR)/index.html"

.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -rf $(DOXYGEN_OUTPUT_DIR)
	@rm -f *.o *.log
	@echo "Clean complete!"

.PHONY: help
help:
	@echo "Available commands:"
	@echo "  make compile_run           - Compile and run Iteration_$(LATEST_ITERATION)"
	@echo "  make build_image           - Build the Docker image"
	@echo "  make run_docker            - Run Docker containers and collect metrics"
	@echo "  make run_local_analysis    - Run all iterations locally and collect metrics"
	@echo "  make generate_docs         - Generate documentation with Doxygen"
	@echo "  make clean                 - Clean up generated files and artifacts"
