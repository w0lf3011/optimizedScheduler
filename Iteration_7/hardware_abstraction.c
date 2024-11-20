#include "hardware_abstraction.h"
#include <stdio.h>


void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

void delay(uint32_t ms) {
    sleep_ms(ms);
}

void enter_deep_sleep(uint32_t duration_ms) {
    printf("Entering deep sleep for %u ms.\n", duration_ms);
    sleep_ms(duration_ms);
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