#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

/// \brief GPIO pin number assigned to the LED.
#define LED_PIN 25

/**
 * \brief Initializes hardware peripherals.
 * 
 * This function is responsible for initializing the hardware peripherals.
 * In a simulation environment, it outputs a message indicating initialization.
 * 
 * \note This function is a placeholder for actual hardware initialization in an embedded system.
 */
void init_peripherals();

/**
 * \brief Delays execution for a specified duration.
 * 
 * This function pauses execution for the given duration in milliseconds.
 * 
 * \param ms Duration of the delay in milliseconds.
 */
void delay(uint32_t ms);

/**
 * \brief Simulates entering deep sleep mode.
 * 
 * This function simulates a deep sleep state to conserve energy by delaying
 * execution for the specified duration.
 * 
 * \param duration_ms Duration of the deep sleep in milliseconds.
 */
void enter_deep_sleep(uint32_t duration_ms);

/**
 * \brief Simulates blinking an LED a specified number of times.
 * 
 * This function outputs messages to simulate the LED turning on and off.
 * 
 * \param times Number of times the LED should blink.
 */
void blink_led(uint8_t times);

#endif // HARDWARE_ABSTRACTION_H
