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

/// \brief Task priority levels.
#define PRIORITY_HIGH 3
#define PRIORITY_MEDIUM 2
#define PRIORITY_LOW 1

/**
 * \brief Task structure with priority.
 */
typedef struct {
    void (*taskFunction)();  ///< Pointer to the task function.
    uint32_t delay_ms;       ///< Delay before executing the task, in milliseconds.
    uint8_t priority;        ///< Priority of the task.
} Task;

/// \brief Array for storing temperature values.
float temperature_values[NUM_MEASURES];

/// \brief Index of the current temperature measurement in the buffer.
uint8_t measure_index = 0;

/// \brief Indicates whether the circular buffer is full.
bool buffer_full = false;

/// \brief Global counter for executed tasks.
uint8_t global_taskcounter = 0;

/**
 * \brief Increments the global task counter and logs the task name.
 * 
 * \param taskName Name of the task being executed.
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
 * \brief Comparison function to sort tasks by priority (highest first).
 * 
 * \param a Pointer to the first task.
 * \param b Pointer to the second task.
 * \return A positive value if task `b` has a higher priority, a negative value if `a` does, or 0 if equal.
 */
int compare_tasks(const void* a, const void* b) {
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    return (taskB->priority - taskA->priority); // Descending order of priority.
}

/**
 * \brief Executes tasks in the order of their priority.
 * 
 * \param tasks Array of tasks.
 * \param task_count Number of tasks in the array.
 */
void task_scheduler(Task* tasks, uint8_t task_count) {
    // Sort tasks by priority
    qsort(tasks, task_count, sizeof(Task), compare_tasks);
    
    // Execute each task in order of priority
    for (uint8_t i = 0; i < task_count; i++) {
        tasks[i].taskFunction();     // Execute the task
        sleep_ms(tasks[i].delay_ms); // Delay after each task
    }
}

/**
 * \brief Main entry point of the program.
 * 
 * This function initializes peripherals, sets up tasks, and runs them in a loop. The loop count can be specified as a command-line argument.
 * 
 * \param argc Number of command-line arguments.
 * \param argv Array of command-line arguments. The first argument specifies the loop count (0 for infinite loop).
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    init_peripherals();
    
    int loop_count = 0;
    bool infinite_loop = true;

    if (argc > 1) {
        loop_count = atoi(argv[1]); // Convert the argument to an integer.
        if (loop_count < 0) {
            printf("Invalid argument. Usage: %s <loop count>.\n", argv[0]);
            return 1;
        } else {
            infinite_loop = (loop_count == 0);
        }
    }
	
    // Define tasks with their functions, delays, and priorities
    Task tasks[] = {
        { runTempTask, 5000, PRIORITY_LOW },
        { computeAvgTempTask, 5000, PRIORITY_MEDIUM },
        { sendResultTask, 1000, PRIORITY_HIGH }
    };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);
    
    while (infinite_loop || loop_count > 0) {
        task_scheduler(tasks, task_count);
        sleep_ms(1000);
        loop_count--;
    }
}
