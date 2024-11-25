# OptimizedScheduler: Energy-Harvesting Sensor Systems Scheduler

This repository contains the code and documentation for a Master's thesis project focused on developing an optimal scheduler for energy-harvesting sensor systems. The scheduler is designed to efficiently manage tasks in environments where sensors operate without batteries, relying solely on harvested energy.

## Features

- **Task Prioritization**: Implements a priority-based system to ensure critical tasks are executed promptly.
- **Energy Management**: Incorporates algorithms that adapt to varying energy availability, optimizing task execution based on current energy levels.
- **Scalability**: Supports dynamic queue management to handle varying numbers of tasks and devices.
- **Fault Tolerance**: Includes mechanisms to maintain system stability despite intermittent energy supply.
- **Machine Learning Integration**: Utilizes trained models to predict energy availability and adjust scheduling accordingly.

## Repository Structure

- `Iteration_0` to `Iteration_7`: Directories containing code and documentation for each development iteration, showcasing the evolution of the scheduler.
- `doc/soa`: Documentation files, including the state-of-the-art analysis related to the project.
- `.gitignore`: Specifies files and directories to be ignored by Git.
- `LICENSE`: The project's licensing information.
- `README.md`: This file, providing an overview of the project.
- `build_docker_images.sh`: Script to build Docker images for the project.
- `monitor_metrics.py`: Python script for monitoring system metrics during scheduler operation.
- `run_docker_containers.sh`: Script to run Docker containers for testing and deployment.

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

3. **Build Docker Images** (if applicable):
   ```bash
   ./build_docker_images.sh
   ```

4. **Run Docker Containers**:
   ```bash
   ./run_docker_containers.sh
   ```

5. **Monitor Metrics**:
   ```bash
   python monitor_metrics.py
   ```


## Documentation
Detailed documentation for each iteration is available within the respective directories. The doc/soa directory contains the state-of-the-art analysis and other relevant documents.


## Contributing
Contributions are welcome. Please fork the repository and submit a pull request with your proposed changes.

## License
This project is licensed under the GNU General Public License v2.0. See the LICENSE file for more details.
