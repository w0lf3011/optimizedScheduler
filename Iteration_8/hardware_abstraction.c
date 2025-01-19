#include "hardware_abstraction.h"

/**
 * \brief Initializes hardware peripherals.
 * 
 * This function simulates the initialization of hardware peripherals,
 * such as GPIO, I2C, or other components, for a Linux environment.
 * 
 * \note In a real embedded system, this function would include actual
 * hardware initialization code.
 */
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

/**
 * \brief Delays the execution for a specified duration.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void delay(uint32_t ms) {
    sleep_ms(ms);
}

/**
 * \brief Simulates entering a deep sleep mode for energy conservation.
 * 
 * This function simulates a deep sleep mode by delaying the execution for
 * the specified duration.
 * 
 * \param duration_ms Duration of the deep sleep in milliseconds.
 */
void enter_deep_sleep(uint32_t duration_ms) {
    printf("Entering deep sleep for %u ms.\n", duration_ms);
    sleep_ms(duration_ms);
}

/**
 * \brief Simulates blinking an LED a specified number of times.
 * 
 * This function prints messages to simulate the LED turning on and off.
 * 
 * \param times Number of times the LED should blink.
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
 * \brief Delays the execution for a specified duration in milliseconds.
 * 
 * This function converts the given duration from milliseconds to microseconds
 * and calls `usleep` to perform the delay.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convert milliseconds to microseconds
}
