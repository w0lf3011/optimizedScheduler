#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SOLAR,
    WIND,
    BATTERY
} EnergyType;

typedef struct {
    EnergyType type;
    uint8_t start_hour;
    uint8_t duration_hours;
    uint8_t occurrences_per_day;
    uint8_t availability_history[5];
    float predictability;
} EnergySource;

void update_energy_profile(EnergySource *source);
bool is_energy_available(EnergySource *source, uint8_t hours);

#endif
