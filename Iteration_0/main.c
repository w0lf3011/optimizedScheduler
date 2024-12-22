#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

/// \brief Number of temperature measurements to perform.
#define NUM_MEASURES 10

/// \brief GPIO pin for the LED.
#define LED_PIN 25

/// \brief I2C address of the temperature sensor (example).
#define TEMP_SENSOR_ADDR 0x48

/// \brief Array to store temperature values.
float temperature_values[NUM_MEASURES];
/// \brief Current number of temperature measurements taken.
uint8_t measure_count = 0;
/// \brief Global counter for executed tasks.
uint8_t global_taskcounter = 0;

/**
 * \brief Increments the global task counter and prints the name of the last executed task.
 * 
 * \param taskName Name of the executed task.
 */
void incrementTaskCounter(const char *taskName) {
    global_taskcounter++;
    printf("%d tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}

/**
 * \brief Initializes the necessary peripherals (simulated for Linux).
 */
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

/**
 * \brief Reads a simulated temperature value from the sensor.
 * 
 * \return The simulated temperature in degrees Celsius.
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
    for (uint8_t i = 0; i < measure_count; i++) {
        sum += temperature_values[i];
    }
    return (measure_count > 0) ? (sum / measure_count) : 0;
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
 * \brief Simulates a sleep for the specified number of milliseconds.
 * 
 * \param ms Time to sleep in milliseconds.
 */
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Converts milliseconds to microseconds
}

/**
 * \brief Task to retrieve the current temperature value from the sensor.
 */
void runTempTask() {
    if (measure_count < NUM_MEASURES) {
        temperature_values[measure_count] = read_temperature();
        measure_count++;
        incrementTaskCounter("runTempTask");
    }
}

/**
 * \brief Task to compute the average of the temperatures stored in memory.
 */
void computeAvgTempTask() {
    if (measure_count == NUM_MEASURES) {
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

/**
 * \brief Task to simulate sending the result by blinking the LED a number of times equal to the collected measurements.
 */
void sendResultTask() {
    if (measure_count == NUM_MEASURES) {
        blink_led(measure_count);
        measure_count = 0;  // Reset the measurement counter
        incrementTaskCounter("sendResultTask");
    }
}

/**
 * \brief Scheduler function to manage task execution in sequence.
 */
void task_scheduler() {
    runTempTask();
    sleep_ms(1000);
    runTempTask();
    sleep_ms(1000);
    runTempTask();
    sleep_ms(1000);
    computeAvgTempTask();
    sendResultTask();
}

/**
 * \brief Main entry point of the program.
 * 
 * This function initializes peripherals and executes tasks in a loop. The loop count can be specified as an argument.
 * 
 * \param argc Number of arguments passed to the program.
 * \param argv Array of arguments passed to the program. The first argument specifies the loop count (0 for infinite loop).
 * \return Exit status code.
 */
int main(int argc, char *argv[]) {
    init_peripherals();  // Initialize peripherals

    int loop_count = 0;
    bool infinite_loop = true;

    if (argc > 1) {
        loop_count = atoi(argv[1]); // Convert argument to integer
        if (loop_count < 0) {
            printf("Invalid argument. Usage: %s <loop count>.\n", argv[0]);
            return 1;
        } else {
            infinite_loop = (loop_count == 0);
        }
    }
	
    while (infinite_loop || loop_count > 0) {
        task_scheduler();
        sleep_ms(1000);
        loop_count--;
    }
}
