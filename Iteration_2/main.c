#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/// \brief Number of stored temperature measurements.
#define NUM_MEASURES 10

/// \brief GPIO pin for the LED.
#define LED_PIN 25

/// \brief I2C address of the temperature sensor (example).
#define TEMP_SENSOR_ADDR 0x48

/**
 * \brief Task structure with priority, weight, and dependencies.
 */
typedef struct Task {
    void (*taskFunction)();      ///< Pointer to the task function.
    uint32_t delay_ms;           ///< Delay before executing the task in milliseconds.
    uint8_t priority;            ///< Priority of the task (higher value = higher priority).
    uint8_t weight;              ///< Weight representing task execution cost or duration.
    struct Task** dependencies;  ///< Pointers to dependent tasks.
    uint8_t num_dependencies;    ///< Number of dependencies.
} Task;

/// \brief Circular buffer for storing temperature values.
float temperature_values[NUM_MEASURES];
/// \brief Current index in the circular buffer.
uint8_t measure_index = 0;
/// \brief Indicates whether the circular buffer is full.
bool buffer_full = false;
/// \brief Global counter for executed tasks.
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
 * \brief Initializes necessary peripherals (simulated for Linux).
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
    printf("Read a temperature of %.f\n", temp_1d);
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
        usleep(200000);  // 200 ms
        printf("LED OFF\n");
        usleep(200000);
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
    if (buffer_full) {  // Calculate only when the buffer is full.
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

/**
 * \brief Task to send results, simulated by blinking the LED.
 */
void sendResultTask() {
    if (buffer_full) {  // Blink only when the buffer is full.
        blink_led(NUM_MEASURES);
        buffer_full = false;
        measure_index = 0;
        incrementTaskCounter("sendResultTask");
    }
}

/**
 * \brief Compares two tasks based on priority and weight for sorting.
 * 
 * \param a Pointer to the first task.
 * \param b Pointer to the second task.
 * \return A positive value if task `b` has higher priority or lower weight, negative otherwise.
 */
int compare_tasks(const void* a, const void* b) {
    Task* taskA = *(Task**)a;
    Task* taskB = *(Task**)b;
    if (taskA->priority != taskB->priority) {
        return taskB->priority - taskA->priority;
    }
    return taskA->weight - taskB->weight;
}

/**
 * \brief Executes tasks in order of priority and dependencies.
 * 
 * \param tasks Array of task pointers.
 * \param task_count Number of tasks in the array.
 */
void execute_tasks(Task* tasks[], uint8_t task_count) {
    qsort(tasks, task_count, sizeof(Task*), compare_tasks);

    bool task_completed[task_count];
    for (uint8_t i = 0; i < task_count; i++) {
        task_completed[i] = false;
    }

    for (uint8_t i = 0; i < task_count; i++) {
        Task* task = tasks[i];
        bool ready_to_run = true;

        // Check dependencies
        for (uint8_t j = 0; j < task->num_dependencies; j++) {
            Task* dependency = task->dependencies[j];
            if (!task_completed[i]) {
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
 * Initializes peripherals, sets up tasks, and executes them in a loop. Accepts an optional loop count argument.
 * 
 * \param argc Number of command-line arguments.
 * \param argv Array of command-line arguments. The first argument specifies the loop count (0 for infinite loop).
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    init_peripherals();

    int loop_count = (argc > 1) ? atoi(argv[1]) : 0;
    bool infinite_loop = (loop_count == 0);

    // Define tasks with their functions, delays, priorities, weights, and dependencies.
    static Task runTempTaskStruct = { runTempTask, 5000, 3, 1, NULL, 0 };
    static Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 2, 2, NULL, 0 };
    static Task sendResultTaskStruct = { sendResultTask, 1000, 1, 3, NULL, 0 };

    static Task* dependencies[] = { &computeAvgTempTaskStruct };
    sendResultTaskStruct.dependencies = dependencies;
    sendResultTaskStruct.num_dependencies = 1;

    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    while (infinite_loop || loop_count > 0) {
        execute_tasks(tasks, task_count);
        sleep_ms(1000);
        loop_count--;
    }
}
