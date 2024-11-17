#include "hardware_abstraction.h"
#include "pico/stdlib.h"
#include <stdio.h>


void init_peripherals() {
    stdio_init_all();
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
    for (uint8_t i = 0; i < times; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(200);
        gpio_put(LED_PIN, 0);
        sleep_ms(200);
    }
}