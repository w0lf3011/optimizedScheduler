#include "energy_manager.h"
#include "hardware_abstraction.h"

#define HISTORY_DAYS 5 


void update_energy_profile(EnergySource *source) {
    // Logic to update the profile of energy based on historical data
    uint8_t available_hours_today = source->duration_hours * source->occurrences_per_day;
    for (int i = HISTORY_DAYS - 1; i > 0; i--) {
        source->availability_history[i] = source->availability_history[i - 1];
    }
    source->availability_history[0] = available_hours_today;

    // Calcul de la prévisibilité en utilisant l'historique
    float total = 0;
    float variance = 0;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        total += source->availability_history[i];
    }
    float mean = total / HISTORY_DAYS;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        variance += (source->availability_history[i] - mean) * (source->availability_history[i] - mean);
    }
    source->predictability = 1.0f / (1.0f + (variance / HISTORY_DAYS));
}

bool is_energy_available(EnergySource *source) {
    // Logic to check availability based on the current simulated hour and the profile
    uint8_t current_hour = get_current_hour();
    uint8_t interval_hours = 24 / source->occurrences_per_day;
    
    for (uint8_t i = 0; i < source->occurrences_per_day; i++) {
        uint8_t start_time = (source->start_hour + i * interval_hours) % 24;
        uint8_t end_time = (start_time + source->duration_hours) % 24;

        if (start_time < end_time) {
            if (current_hour >= start_time && current_hour < end_time) {
                return true;
            }
        } else {
            if (current_hour >= start_time || current_hour < end_time) {
                return true;
            }
        }
    }
    return false;
}
