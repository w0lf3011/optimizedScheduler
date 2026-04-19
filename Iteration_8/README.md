## Overview
`Iteration_8` is the current reference iteration of the project. It simulates an energy-aware embedded scheduler that:

- checks whether harvested energy is available for the current simulated hour,
- updates a predictability score from recent energy history,
- prioritizes tasks with a heuristic metric,
- enforces task dependencies before execution,
- simulates hardware actions such as deep sleep and LED feedback.

## Main Flow
The executable starts in `main.c` and performs the following steps:

1. Initializes the simulated hardware layer.
2. Builds a queue for the current task cycle.
3. Declares three sample tasks:
   - temperature acquisition,
   - average computation,
   - result transmission.
4. Recomputes task heuristic scores when energy is available.
5. Enqueues the tasks and executes them while respecting dependencies.
6. Advances the simulated time hour by hour.

## Build And Run
Compile locally with:

```bash
gcc -Wall -Wextra -pedantic -o bin/iteration_8 Iteration_8/*.c
```

Run a one-day simulation:

```bash
./bin/iteration_8 1
```

Run indefinitely:

```bash
./bin/iteration_8 0
```

If no argument is provided, the program runs a one-day simulation by default.

## File Roles
- `main.c`: entry point, sample tasks, dependency wiring, simulation loop.
- `task_manager.c/.h`: task model, priority queue, dependency-aware execution.
- `energy_manager.c/.h`: energy source profile, predictability update, availability checks, TinyML placeholders.
- `hardware_abstraction.c/.h`: simulated board services.
- `error_handling.c/.h`: centralized error reporting.

## Notes
- TinyML support is currently a placeholder and returns a fixed prediction value.
- The scheduler logic is simulation-oriented rather than tied to a specific MCU SDK.
- The current demo tasks illustrate ordering and dependency handling more than production sensor logic.
