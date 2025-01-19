# OptimizedScheduler: Embedded Energy-Aware Task Scheduler

## Overview
This project implements an energy-aware task scheduler for embedded systems, capable of prioritizing tasks based on their criticality and energy availability. The system integrates energy profiling, heuristic-based task management, and hardware abstraction, and supports TinyML models for advanced energy prediction.


## Features

- **Energy Source Management**: Profiles energy sources (e.g., solar, wind, battery) with predictability based on historical data.
- **Task Scheduler**:
  - Heuristic-based priority scheduling.
  - Dependency management.
  - Critical task prioritization.
- **TinyML Integration**:
  - Placeholder for TensorFlow Lite Micro integration for energy prediction.
- **Hardware Abstraction**:
  - Simulates peripherals like LEDs and deep sleep modes.
- **Error Handling**:
  - Centralized error reporting with support for future recovery mechanisms.


## Repository Structure

- `Iteration_0` to `Iteration_7`: Directories containing code and documentation for each development iteration, showcasing the evolution of the scheduler.
- `doc/soa`: Documentation files, including the state-of-the-art analysis related to the project.
- `Doxyfile`: Configuration for Doxygen
- `LICENSE`: The project's licensing information.
- `README.md`: This file, providing an overview of the project.
- `build_docker_images.sh`: Script to build Docker images for the project.
- `monitor_metrics.py`: Python script for monitoring system metrics during scheduler operation.
- `run_docker_containers.sh`: Script to run Docker containers for testing and deployment.


## Installation

### Prerequisites
- **Compiler**: A C compiler such as GCC.
- **Python**: Use to run some script to interact with Docker stats module.
- **Doxygen**: For generating documentation (optional).
- **Make**: To simplify build tasks (optional). Not in use here, but definitivelly a point to better structure the run.


## Getting Started

To explore or contribute to the project:

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/w0lf3011/optimizedScheduler.git
   ```

2. **Navigate to the Desired Iteration**:
   ```bash
   cd optimizedScheduler/Iteration_X
   ```

### Via Docker

1. **Build Docker Images** (if applicable):
   ```bash
   ./build_docker_images.sh
   ```

2. **Run Docker Containers**:
   ```bash
   ./run_docker_containers.sh
   ```

3. **Monitor Metrics**:
   ```bash
   python monitor_metrics.py
   ```

### Run locally an instance

1. **Compile the project**
   ```bash
   gcc -o scheduler src/*.c -I include
   ```

2. **Run the program**
   ```bash
   ./scheduler [duration_days]
   ```
   [duration_days] Number of simulation days (default is infinite if omitted).


### Run locally all Iterations multiple times
This run option will incremently run all iterations until the last one configured, and for each of them will repeat X times. As the result, Thread time will be collected, calculate it to convert it in Power (watts) will generate a dataset as a result in JSON, TEX and CSV files into the folder `output`.

1. **Configure the parameters of `run_analysis_local.sh` script**
   ```bash
   ITERATIONS=7
   REPETITIONS=5
   TDP=95
   NUM_THREADS=16
   ```
   As my CPU is an Intel I9 9900k, I've set the TDP to 95 watts, and a total of Threads of 16 - [Intel I9 9900k specifications](https://www.intel.com/content/www/us/en/products/sku/186605/intel-core-i99900k-processor-16m-cache-up-to-5-00-ghz/specifications.html)

2. **Run locally all iterations multiple times**
   ```bash
   ./run_analysis_local.sh
   ```





## Documentation
Detailed documentation for each iteration is available within the respective directories. The `doc/soa` directory contains the state-of-the-art analysis and other relevant documents.


## Contributing
Contributions are welcome. Please fork the repository and submit a pull request with your proposed changes.

## License
This project is licensed under the GNU General Public License v2.0. See the `LICENSE` file for more details.
