#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

/// \brief Number of stored temperature measurements.
#define NUM_MEASURES 10

/// \brief GPIO pin for the LED.
#define LED_PIN 25

/// \brief I2C address of the temperature sensor (example).
#define TEMP_SENSOR_ADDR 0x48

/// \brief Default simulation duration in days.
#define DEFAULT_SIMULATION_DAYS 7

/**
 * \brief Types of energy sources.
 */
typedef enum {
    SOLAR,   ///< Solar energy.
    WIND,    ///< Wind energy.
    BATTERY  ///< Battery energy.
} EnergyType;

/**
 * \brief Types of simulation goals.
 */
typedef enum {
    MAXIMIZE_TASKS,     ///< Goal: Maximize the number of tasks executed.
    MAXIMIZE_RESILIENCE ///< Goal: Maximize resilience by prioritizing critical tasks.
} GoalType;

/**
 * \brief Structure defining an energy source.
 */
typedef struct {
    EnergyType type;            ///< Type of energy source.
    uint8_t start_hour;         ///< Start hour of the first occurrence (0-23).
    uint8_t duration_hours;     ///< Duration of each active period in hours.
    uint8_t occurrences_per_day; ///< Number of times the energy source is available per day.
} EnergySource;

/**
 * \brief Structure defining goal parameters.
 */
typedef struct {
    GoalType goal;               ///< Selected simulation goal.
    uint8_t duration_days;       ///< Duration of the simulation in days.
} GoalParameters;

/**
 * \brief Structure representing a task with priority, weight, and dependencies.
 */
typedef struct Task {
    void (*taskFunction)();     ///< Pointer to the task function.
    uint32_t delay_ms;          ///< Delay before executing the task, in milliseconds.
    uint8_t priority;           ///< Task priority (higher value = higher priority).
    uint8_t weight;             ///< Weight representing execution cost or duration.
    bool critical;              ///< Indicates if the task is critical for resilience.
    struct Task** dependencies; ///< Pointers to dependent tasks.
    uint8_t num_dependencies;   ///< Number of dependencies.
} Task;

float temperature_values[NUM_MEASURES]; ///< Circular buffer for storing temperature values.
uint8_t measure_index = 0;              ///< Current index in the circular buffer.
bool buffer_full = false;               ///< Indicates whether the circular buffer is full.
uint8_t simulated_hour = 5;             ///< Simulated current hour (0-23).
uint8_t simulated_day = 0;              ///< Simulated current day for the simulation.

GoalParameters global_goal_params; ///< Global variable for storing goal parameters.
uint8_t global_taskcounter = 0;    ///< Global counter for executed tasks.

/**
 * \brief Increments the global task counter and logs the task name.
 * 
 * \param taskName Name of the executed task.
 */
void incrementTaskCounter(const char *taskName) {
    global_taskcounter++;
    printf("%d tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}

/**
 * \brief Initializes peripherals (simulated for Linux).
 */
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

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
    printf("Read a temperature of %.f°C\n", temp_1d);
    return temp_1d;
}

/**
 * \brief Calculates the average of the stored temperature values.
 * 
 * \return The average temperature value, or 0 if no measurements have been taken.
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
 * \brief Simulates blinking an LED a specified number of times.
 * 
 * \param times Number of times to blink the LED.
 */
void blink_led(uint8_t times) {
    printf("Simulating blinking LED %d times...\n", times);
    for (uint8_t i = 0; i < times; i++) {
        printf("LED ON\n");
        usleep(2000);
        printf("LED OFF\n");
        usleep(2000);
    }
}

/**
 * \brief Simulates a sleep for the specified duration in milliseconds.
 * 
 * \param ms Sleep duration in milliseconds.
 */
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convert milliseconds to microseconds.
}

/**
 * \brief Task to retrieve the current temperature from the sensor.
 */
void runTempTask() {
    temperature_values[measure_index] = read_temperature();
    measure_index = (measure_index + 1) % NUM_MEASURES;
    if (measure_index == 0) buffer_full = true;
    incrementTaskCounter("runTempTask");
}

/**
 * \brief Task to compute the average of the collected temperatures.
 */
void computeAvgTempTask() {
    if (buffer_full) {
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

/**
 * \brief Task to send results, simulated by blinking the LED.
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
 * \brief Updates the simulated current hour.
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
 * \return The simulated current hour (0-23).
 */
uint8_t get_current_hour() {
    return simulated_hour;
}

/**
 * \brief Checks if the specified energy source is available.
 * 
 * \param source Pointer to the energy source.
 * \return True if the energy source is available; otherwise, false.
 */
bool is_energy_available(EnergySource *source) {
    uint8_t current_hour = get_current_hour();
    uint8_t interval_hours = 24 / source->occurrences_per_day;
    
    for (uint8_t i = 0; i < source->occurrences_per_day; i++) {
        uint8_t start_time = (source->start_hour + i * interval_hours) % 24;
        uint8_t end_time = (start_time + source->duration_hours) % 24;

        if (start_time < end_time) {
            if (current_hour >= start_time && current_hour < end_time) {
                return true;
            }
        } else {
            if (current_hour >= start_time || current_hour < end_time) {
                return true;
            }
        }
    }
    return false;
}

/**
 * \brief Compares tasks based on goal parameters, priority, and weight.
 * 
 * \param a Pointer to the first task.
 * \param b Pointer to the second task.
 * \return Comparison result for sorting.
 */
int compare_tasks(const void* a, const void* b) {
    Task* taskA = *(Task**)a;
    Task* taskB = *(Task**)b;

    if (global_goal_params.goal == MAXIMIZE_RESILIENCE) {
        if (taskA->critical != taskB->critical) {
            return taskB->critical - taskA->critical;
        }
    }

    if (taskA->priority != taskB->priority) {
        return taskB->priority - taskA->priority;
    }
    return taskA->weight - taskB->weight;
}

/**
 * \brief Executes tasks based on the selected goal and energy availability.
 * 
 * \param tasks Array of task pointers.
 * \param task_count Number of tasks in the array.
 * \param source Pointer to the energy source.
 * \param goal_params Pointer to the goal parameters.
 */
void execute_tasks(Task* tasks[], uint8_t task_count, EnergySource *source, GoalParameters *goal_params) {
    global_goal_params = *goal_params;

    if (!is_energy_available(source)) {
        printf("Energy source is not available. Tasks are paused.\n");
        return;
    }

    qsort(tasks, task_count, sizeof(Task*), compare_tasks);

    bool task_completed[task_count];
    for (uint8_t i = 0; i < task_count; i++) {
        task_completed[i] = false;
    }

    for (uint8_t i = 0; i < task_count; i++) {
        Task* task = tasks[i];
        bool ready_to_run = true;

        for (uint8_t j = 0; j < task->num_dependencies; j++) {
            if (!task_completed[j]) {
                ready_to_run = false;
                break;
            }
        }

        if (ready_to_run) {
            task->taskFunction();
            task_completed[i] = true;
            sleep_ms(task->delay_ms);
        }
    }
}

/**
 * \brief Main entry point of the program.
 * 
 * Initializes peripherals, sets up tasks, and executes them in a loop based on energy availability.
 * 
 * \param argc Number of command-line arguments.
 * \param argv Array of command-line arguments. The first argument specifies the simulation duration (in days, 0 for infinite loop).
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    init_peripherals();

    int duration_days = (argc > 1) ? atoi(argv[1]) : DEFAULT_SIMULATION_DAYS;
    bool infinite_loop = (duration_days == 0);

    static Task runTempTaskStruct = { runTempTask, 5000, 3, 1, NULL, 0 };
    static Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 2, 2, NULL, 0 };
    static Task sendResultTaskStruct = { sendResultTask, 1000, 1, 3, NULL, 0 };

    static Task* dependencies[] = { &computeAvgTempTaskStruct };
    sendResultTaskStruct.dependencies = dependencies;
    sendResultTaskStruct.num_dependencies = 1;

    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    EnergySource energy_source = { SOLAR, 6, 12, 1 };
    GoalParameters goal_params = { MAXIMIZE_RESILIENCE, duration_days };

    while (infinite_loop || simulated_day < goal_params.duration_days) {
        execute_tasks(tasks, task_count, &energy_source, &goal_params);
        sleep_ms(1000);
        update_simulated_hour();
    }

    printf("Simulation completed over %d days.\n", goal_params.duration_days);
    return 0;
}
