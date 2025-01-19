#include "task_manager.h"
#include "energy_manager.h"
#include "hardware_abstraction.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define NUM_MEASURES 10 ///< Number of stored temperature measurements.

/////////////////////
/// Global variables
/////////////////////

/**
 * \brief Array to store temperature measurements.
 */
float temperature_values[NUM_MEASURES];

/**
 * \brief Current index in the temperature measurements array.
 */
uint8_t measure_index = 0;

/**
 * \brief Flag indicating if the buffer is full.
 */
bool buffer_full = false;

/**
 * \brief Simulated current hour (0-23).
 */
uint8_t simulated_hour = 5;

/**
 * \brief Simulated current day.
 */
uint8_t simulated_day = 0;

/**
 * \brief Global parameters for the simulation goal.
 */
GoalParameters global_goal_params;

/**
 * \brief Counter for the total number of executed tasks.
 */
uint8_t global_taskcounter = 0;

/////////////////////
/// Helper Functions
/////////////////////

/**
 * \brief Increments the global task counter and logs the task name.
 * 
 * \param taskName The name of the executed task.
 */
void incrementTaskCounter(const char *taskName) {
    global_taskcounter++;
    printf("%d tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}

/////////////////////
/// Implementation of Tasks
/////////////////////

/**
 * \brief Reads a simulated temperature value from the sensor.
 * 
 * \return A temperature value in degrees Celsius.
 */
float read_temperature() {
    float min = -40.0f;
    float max = 60.0f;

    float temp = min + ((float)rand() / (float)RAND_MAX) * (max - min);
    float temp_1d = ((int)(temp * 10)) / 10.0f;
    printf("Read a temperature of %.f\n", temp_1d);
    return temp_1d;
}

/**
 * \brief Calculates the average of the stored temperature values.
 * 
 * \return The average temperature value.
 */
float average() {
    float sum = 0.0;
    uint8_t count = buffer_full ? NUM_MEASURES : measure_index;
    for (uint8_t i = 0; i < count; i++) {
        sum += temperature_values[i];
    }
    return (count > 0) ? (sum / count) : 0;
}

/**
 * \brief Task to read the current temperature from the sensor.
 */
void runTempTask() {
    temperature_values[measure_index] = read_temperature();
    measure_index = (measure_index + 1) % NUM_MEASURES;
    if (measure_index == 0) buffer_full = true;
    incrementTaskCounter("runTempTask");
}

/**
 * \brief Task to compute the average temperature from collected data.
 */
void computeAvgTempTask() {
    if (buffer_full) {
        float average_temp = average();
        printf("Average of collected temperatures: %.2f C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

/**
 * \brief Task to send computed data, simulated by blinking an LED.
 */
void sendResultTask() {
    if (buffer_full) {
        blink_led(NUM_MEASURES);
        buffer_full = false;
        measure_index = 0;
        incrementTaskCounter("sendResultTask");
    }
}

/**
 * \brief Updates the simulated hour and day.
 */
void update_simulated_hour() {
    simulated_hour = (simulated_hour + 1) % 24;
    if (simulated_hour == 0) {
        simulated_day++;
    }
}

/**
 * \brief Gets the current simulated hour.
 * 
 * \return The current simulated hour (0-23).
 */
uint8_t get_current_hour() {
    return simulated_hour;
}

/**
 * \brief Main entry point of the program.
 * 
 * This function initializes peripherals, sets up tasks and energy sources,
 * and manages the execution of tasks dynamically based on energy availability
 * and simulation goals.
 * 
 * \param argc Number of command-line arguments.
 * \param argv Array of command-line arguments. The first argument specifies 
 *             the simulation duration in days (0 for infinite loop).
 * 
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    // Initialize peripherals and task queue
    init_peripherals();
    TaskQueue* queue = init_task_queue(MAX_TASKS);

    // Parse simulation duration
    int duration_days = (argc > 1) ? atoi(argv[1]) : 0;
    bool infinite_loop = (duration_days == 0);

    // Initialize tasks
    static Task runTempTaskStruct = { runTempTask, 5000, 2, 3, false, 0.0, NULL, 0 };
    static Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 1, 2, true, 0.0, NULL, 0 };
    static Task sendResultTaskStruct = { sendResultTask, 1000, 3, 1, true, 0.0, NULL, 0 };

    static Task* dependencies[] = { &computeAvgTempTaskStruct };
    sendResultTaskStruct.dependencies = dependencies;
    sendResultTaskStruct.num_dependencies = 1;

    enqueue_task(queue, &runTempTaskStruct);
    enqueue_task(queue, &computeAvgTempTaskStruct);
    enqueue_task(queue, &sendResultTaskStruct);

    // Initialize energy source and parameters
    EnergySource energy_source = { SOLAR, 6, 12, 1, {0}, 0.0 };
    GoalParameters goal_params = { MAXIMIZE_RESILIENCE, duration_days };

    // Main simulation loop
    while (infinite_loop || simulated_day < goal_params.duration_days) {
        if (is_energy_available(&energy_source, get_current_hour())) {
            update_energy_profile(&energy_source);
            execute_tasks(queue, &energy_source, &goal_params);
        } else {
            enter_deep_sleep(5000);
        }
        update_simulated_hour();
    }

    // Free task queue
    free_task_queue(queue);
    return 0;
}
