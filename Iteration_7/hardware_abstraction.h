#include <stdint.h>

#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

#define LED_PIN 25


void init_peripherals();
void delay(uint32_t ms);
void enter_deep_sleep(uint32_t duration_ms);
void blink_led(uint8_t times);

#endif
