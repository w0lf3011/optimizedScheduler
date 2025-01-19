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

/// \brief Number of days for historical availability profile.
#define HISTORY_DAYS 5

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
 * \brief Structure defining an energy source with historical intermittency profile.
 */
typedef struct {
    EnergyType type;              ///< Type of energy source.
    uint8_t start_hour;           ///< Start hour of the first occurrence (0-23).
    uint8_t duration_hours;       ///< Duration of each active period in hours.
    uint8_t occurrences_per_day;  ///< Number of times the energy source is available per day.
    uint8_t availability_history[HISTORY_DAYS]; ///< Historical availability in hours for past days.
    float predictability;         ///< Predictability score based on historical data.
} EnergySource;

/**
 * \brief Structure defining simulation goal parameters.
 */
typedef struct {
    GoalType goal;               ///< Selected simulation goal.
    uint8_t duration_days;       ///< Duration of the simulation in days.
} GoalParameters;

/**
 * \brief Structure representing a task with heuristic metrics.
 */
typedef struct Task {
    void (*taskFunction)();      ///< Pointer to the task function.
    uint32_t delay_ms;           ///< Delay before executing the task, in milliseconds.
    uint8_t priority;            ///< Task priority (higher value = higher priority).
    uint8_t weight;              ///< Task weight representing execution cost or duration.
    bool critical;               ///< Indicates if the task is critical for resilience.
    float heuristic_metric;      ///< Heuristic metric calculated based on task properties.
    struct Task** dependencies;  ///< Pointers to dependent tasks.
    uint8_t num_dependencies;    ///< Number of dependencies.
} Task;

/**
 * \brief Task queue structure to manage tasks dynamically.
 */
typedef struct TaskQueue {
    Task** tasks;    ///< Array of pointers to tasks.
    uint8_t capacity; ///< Maximum capacity of the queue.
    uint8_t size;     ///< Current size of the queue.
} TaskQueue;

/// Circular buffer for storing temperature values.
float temperature_values[NUM_MEASURES];
/// Current index in the circular buffer.
uint8_t measure_index = 0;
/// Indicates whether the circular buffer is full.
bool buffer_full = false;
/// Simulated current hour (0-23).
uint8_t simulated_hour = 5;
/// Simulated current day for the simulation.
uint8_t simulated_day = 0;

/// Global variable for storing goal parameters.
GoalParameters global_goal_params;
/// Global counter for executed tasks.
uint8_t global_taskcounter = 0;

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
 * \brief Initializes a task queue.
 * 
 * \param capacity Maximum number of tasks the queue can hold.
 * \return Pointer to the initialized task queue.
 */
TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

/**
 * \brief Adds a task to the task queue, maintaining order based on heuristic metrics.
 * 
 * \param queue Pointer to the task queue.
 * \param task Pointer to the task to be added.
 */
void enqueue_task(TaskQueue* queue, Task* task) {
    if (queue->size < queue->capacity) {
        queue->tasks[queue->size++] = task;

        for (int i = queue->size - 1; i > 0; i--) {
            if (queue->tasks[i]->heuristic_metric > queue->tasks[i - 1]->heuristic_metric) {
                Task* temp = queue->tasks[i];
                queue->tasks[i] = queue->tasks[i - 1];
                queue->tasks[i - 1] = temp;
            } else {
                break;
            }
        }
    }
}

/**
 * \brief Removes and returns the most prioritized task from the queue.
 * 
 * \param queue Pointer to the task queue.
 * \return Pointer to the dequeued task, or NULL if the queue is empty.
 */
Task* dequeue_task(TaskQueue* queue) {
    if (queue->size == 0) return NULL;
    return queue->tasks[--queue->size];
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
 * \brief Pauses execution for the specified duration in milliseconds.
 *
 * This function uses `usleep` to convert the given duration from milliseconds
 * to microseconds and pauses the execution of the program for that duration.
 *
 * \param ms The sleep duration in milliseconds.
 *
 * \note The actual sleep duration may be slightly longer due to system scheduling delays.
 */
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convert milliseconds to microseconds.
}

/**
 * \brief Simulates entering deep sleep mode to save energy.
 * 
 * \param duration_ms Duration of the deep sleep in milliseconds.
 */
void deep_sleep(uint32_t duration_ms) {
    printf("Entering deep sleep for %u ms due to prolonged inactivity.\n", duration_ms);
    sleep_ms(duration_ms);
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
 * \brief Gets the current simulated hour.
 * 
 * \return The simulated current hour (0-23).
 */
uint8_t get_current_hour() {
    return simulated_hour;
}

/**
 * \brief Updates the energy profile and calculates predictability.
 * 
 * \param source Pointer to the energy source.
 */
void update_energy_profile(EnergySource *source) {
    uint8_t available_hours_today = source->duration_hours * source->occurrences_per_day;
    for (int i = HISTORY_DAYS - 1; i > 0; i--) {
        source->availability_history[i] = source->availability_history[i - 1];
    }
    source->availability_history[0] = available_hours_today;

    // Calcul de la prévisibilité en utilisant l'historique
    float total = 0;
    float variance = 0;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        total += source->availability_history[i];
    }
    float mean = total / HISTORY_DAYS;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        variance += (source->availability_history[i] - mean) * (source->availability_history[i] - mean);
    }
    source->predictability = 1.0f / (1.0f + (variance / HISTORY_DAYS));
}

/**
 * \brief Updates the simulated current hour and updates the energy profile if necessary.
 * 
 * \param source Pointer to the energy source.
 */
void update_simulated_hour(EnergySource *source) {
    simulated_hour = (simulated_hour + 1) % 24;
    if (simulated_hour == 0) {
        simulated_day++;
        update_energy_profile(source);
    }
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
 * \brief Calculates the heuristic metric for a task.
 * 
 * \param task Pointer to the task structure.
 * \param goal_params Pointer to the goal parameters.
 */
void calculate_heuristic_metric(Task *task, GoalParameters *goal_params) {
    if (goal_params->goal == MAXIMIZE_TASKS) {
        task->heuristic_metric = task->priority * 0.5 + task->weight * 0.5;
    } else if (goal_params->goal == MAXIMIZE_RESILIENCE) {
        task->heuristic_metric = task->priority * 0.3 + task->weight * 0.2 + (task->critical ? 1.0 : 0.0) * 0.5;
    }
}


/**
 * \brief Main entry point of the program.
 * 
 * This function initializes peripherals, sets up tasks, and executes them dynamically 
 * based on energy availability and simulation goals. It uses a task queue to manage 
 * execution priorities and incorporates deep sleep for energy-saving when resources are unavailable.
 * 
 * \param argc Number of command-line arguments.
 * \param argv Array of command-line arguments. The first argument specifies the simulation duration 
 *             in days (0 for an infinite loop).
 * 
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    init_peripherals(); ///< Initialize hardware peripherals.

    // Parse simulation duration from command-line arguments
    int duration_days = (argc > 1) ? atoi(argv[1]) : 0;
    bool infinite_loop = (duration_days == 0); ///< Infinite simulation if duration is 0.

    // Define tasks
    static Task runTempTaskStruct = { runTempTask, 5000, 2, 3, false, 0.0, NULL, 0 };
    static Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 1, 2, true, 0.0, NULL, 0 };
    static Task sendResultTaskStruct = { sendResultTask, 1000, 3, 1, true, 0.0, NULL, 0 };

    // Set task dependencies
    static Task* dependencies[] = { &computeAvgTempTaskStruct };
    sendResultTaskStruct.dependencies = dependencies;
    sendResultTaskStruct.num_dependencies = 1;

    // Define task array and count
    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    // Define the energy source
    EnergySource energy_source = { SOLAR, 6, 12, 1, {0}, 0.0 };

    // Set simulation goal parameters
    GoalParameters goal_params = { MAXIMIZE_RESILIENCE, duration_days };

    // Initialize task queue
    TaskQueue* queue = init_task_queue(task_count);

    /**
     * Main simulation loop:
     * - Checks energy availability.
     * - Enqueues tasks based on updated heuristic metrics.
     * - Dynamically executes tasks from the queue.
     * - Enters deep sleep mode if energy is unavailable.
     * - Simulates time progression by updating the simulated hour and day.
     */
    while (infinite_loop || simulated_day < goal_params.duration_days) {
        if (is_energy_available(&energy_source)) {
            // Enqueue tasks based on heuristic metrics
            for (int i = 0; i < task_count; i++) {
                calculate_heuristic_metric(tasks[i], &goal_params);
                enqueue_task(queue, tasks[i]);
            }

            // Execute tasks from the queue
            Task* task;
            while ((task = dequeue_task(queue)) != NULL) {
                task->taskFunction();
                sleep_ms(task->delay_ms); ///< Delay after task execution.
            }
        } else {
            // Enter deep sleep mode if energy is unavailable
            deep_sleep(5000); ///< Simulated deep sleep duration in milliseconds.
        }

        // Update simulated time and control interval
        update_simulated_hour(&energy_source);
        sleep_ms(1000); ///< Control interval simulation.
    }

    // Print simulation completion message
    printf("Simulation completed over %d days.\n", goal_params.duration_days);

    // Clean up dynamically allocated memory
    free(queue->tasks);
    free(queue);

    return 0; ///< Exit with status code 0.
}
