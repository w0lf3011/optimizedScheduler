#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#define NUM_MEASURES 10     // Nombre de mesures à prendre
#define LED_PIN 25          // Pin GPIO pour la LED
#define GPIO_OUT 0
#define TEMP_SENSOR_ADDR 0x48 // Adresse I2C du capteur de température (exemple)

// Variables pour stocker les mesures et les calculs
float temperature_values[NUM_MEASURES];
uint8_t measure_count = 0;
uint8_t global_taskcounter = 0;

// Increment Task Counter and show the task name.
void incrementTaskCounter(const char *taskName) {
    global_taskcounter++;
    printf("%d tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}

// Initialisation des périphériques
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

// Fonction pour lire la température
float read_temperature() {
    // Set the range
    float min = -40.0f;
    float max = 60.0f;

    float temp = min + ((float)rand() / (float)RAND_MAX) * (max - min);
    float temp_1d = ((int)(temp * 10)) / 10.0f;
    printf("Read a temperature of %.f\n", temp_1d);
    return temp_1d;
}

// Fonction pour calculer la moyenne des températures
float average() {
    float sum = 0.0;
    for (uint8_t i = 0; i < measure_count; i++) {
        sum += temperature_values[i];
    }
    return (measure_count > 0) ? (sum / measure_count) : 0;
}

// Fonction pour allumer la LED X fois
void blink_led(uint8_t times) {
    printf("Simulating blinking LED %d times...\n", times);
    for (uint8_t i = 0; i < times; i++) {
        printf("LED ON\n");
        usleep(200000);  // 200 ms
        printf("LED OFF\n");
        usleep(200000);
    }
}

// Fonction pour simuler un sleep
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convertit ms en µs
}

// Task: Get the current value of temperature from the sensor.
void runTempTask() {
    if (measure_count < NUM_MEASURES) {
        temperature_values[measure_count] = read_temperature();
        measure_count++;
        incrementTaskCounter("runTempTask");
    }
}

// Task: Compute the average of temperatures retrieves in volatile memory.
void computeAvgTempTask() {
    if (measure_count == NUM_MEASURES) {
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

// Task: send Result, simulated by the bliking of the LED based on the amount of measure retrieved.
void sendResultTask() {
    if (measure_count == NUM_MEASURES) {
        blink_led(measure_count);
        measure_count = 0;  // Réinitialiser le compteur de mesures
        incrementTaskCounter("sendResultTask");
    }
}


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

int main(int argc, char *argv[]) {
    init_peripherals();  // Initialiser les périphériques

    int loop_count = 0;
    bool infinite_loop = true;

    if (argc > 1) {
        loop_count = atoi(argv[1]); // Convertir l'argument en entier
        if (loop_count < 0) {
            printf("Invalid argument. Usage: %s <loop count>.\n", argv[0]);
            return 1;
        } else {
            infinite_loop = (loop_count == 0);
        }
    }
	
    while (infinite_loop || loop_count>0) {
        task_scheduler();
        sleep_ms(1000);
				loop_count--;
    }
}
