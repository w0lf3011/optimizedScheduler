## Features
- Task Scheduling: Manage tasks with priority and energy constraints.
- Energy Management: Utilize historical data and TinyML predictions.
- TinyML Integration: Load and execute ML models for energy forecasting.
- Fault Tolerance and Scalability: Handle dynamic task queues and intermittent power.

## Getting Started
1. Clone this repository.
2. Add your TinyML model in `main.c`.
3. Compile and execute using the included Makefile.

## Code Architecture
- **energy_manager.c**: Manages energy resources and TinyML integration.
- **task_manager.c**: Handles task queues and dependencies.
- **hardware_abstraction.c**: Simulates hardware-specific functionality.
