#include "hardware_abstraction.h"

/**
 * \brief Simulates the initialization of hardware peripherals.
 * 
 * This function prints a message indicating that hardware peripherals
 * are being initialized. In a real embedded system, this might configure
 * GPIO pins, initialize communication interfaces, or set up timers.
 */
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

/**
 * \brief Simulates a blocking delay for a specified duration.
 * 
 * This function provides a delay in milliseconds by using the `usleep` function,
 * which blocks execution for the given time.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convert milliseconds to microseconds.
}

/**
 * \brief Provides a blocking delay.
 * 
 * This function acts as an alias for `sleep_ms` to provide a blocking delay
 * for a specified duration in milliseconds.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void delay(uint32_t ms) {
    sleep_ms(ms);
}

/**
 * \brief Simulates entering a deep sleep mode for energy conservation.
 * 
 * This function mimics entering a low-power mode where the system is
 * inactive for the specified duration. It prints a message indicating
 * the deep sleep and uses a blocking delay to simulate it.
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
 * This function toggles the state of a simulated LED on and off
 * for a given number of cycles. Each cycle consists of a 200 ms ON state
 * followed by a 200 ms OFF state.
 * 
 * \param times Number of times to blink the LED.
 */
void blink_led(uint8_t times) {
    printf("Simulating blinking LED %d times...\n", times);
    for (uint8_t i = 0; i < times; i++) {
        printf("LED ON\n");
        usleep(200000);  // 200 ms ON
        printf("LED OFF\n");
        usleep(200000);  // 200 ms OFF
    }
}
