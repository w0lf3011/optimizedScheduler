
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define NUM_MEASURES 10     // Nombre de mesures à prendre
#define LED_PIN 25          // Pin GPIO pour la LED
#define GPIO_OUT 0
#define TEMP_SENSOR_ADDR 0x48 // Adresse I2C du capteur de température (exemple)

// Variables pour stocker les mesures et les calculs
float temperature_values[NUM_MEASURES];
uint8_t measure_count = 0;

// Initialisation des périphériques
void init_peripherals() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

// Fonction pour lire la température
float read_temperature() {
    float temp = 25.0;  // Exemple de température simulée
    return temp;
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
    for (uint8_t i = 0; i < times; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(200);  // Délai de 200 ms
        gpio_put(LED_PIN, 0);
        sleep_ms(200);
    }
}

// Task: Get the current value of temperature from the sensor.
void runTempTask() {
    if (measure_count < NUM_MEASURES) {
        temperature_values[measure_count] = read_temperature();
        measure_count++;
    }
}

// Task: Compute the average of temperatures retrieves in volatile memory.
void computeAvgTempTask() {
    if (measure_count == NUM_MEASURES) {
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
    }
}

// Task: send Result, simulated by the bliking of the LED based on the amount of measure retrieved.
void sendResultTask() {
    if (measure_count == NUM_MEASURES) {
        blink_led(measure_count);
        measure_count = 0;  // Réinitialiser le compteur de mesures
    }
}


void task_scheduler() {
    runTempTask();
    sleep_ms(5000);
    runTempTask();
    sleep_ms(5000);
    runTempTask();
    sleep_ms(5000);
    computeAvgTempTask();
    sendResultTask();
}

int main() {
    init_peripherals();  // Initialiser les périphériques

		int counter = 10;
	
    while (counter > 0) {
        task_scheduler();
        sleep_ms(1000);
				counter--;
    }
}
