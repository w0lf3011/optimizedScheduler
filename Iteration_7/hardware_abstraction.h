#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

/// \brief GPIO pin for the LED.
#define LED_PIN 25

/**
 * \brief Initializes hardware peripherals.
 * 
 * This function simulates the initialization of hardware peripherals required
 * for the program. For example, it could configure GPIO pins or initialize
 * I2C interfaces in a real embedded environment.
 */
void init_peripherals();

/**
 * \brief Delays execution for a specified duration in milliseconds.
 * 
 * This function provides a blocking delay to simulate timing behavior in an embedded system.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void delay(uint32_t ms);

/**
 * \brief Enters a simulated deep sleep mode to save energy.
 * 
 * This function mimics entering a low-power deep sleep mode for the specified duration.
 * It is useful for scenarios where energy conservation is critical.
 * 
 * \param duration_ms Duration of the deep sleep in milliseconds.
 */
void enter_deep_sleep(uint32_t duration_ms);

/**
 * \brief Blinks an LED a specified number of times.
 * 
 * This function simulates toggling an LED on and off for the given number of cycles.
 * It is useful for visual feedback or signaling states in an embedded system.
 * 
 * \param times Number of times to blink the LED.
 */
void blink_led(uint8_t times);

#endif // HARDWARE_ABSTRACTION_H
