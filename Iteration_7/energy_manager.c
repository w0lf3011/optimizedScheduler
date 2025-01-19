#include "energy_manager.h"
#include "hardware_abstraction.h"

#define HISTORY_DAYS 5 ///< Number of days considered for historical energy availability.

/**
 * \brief Updates the energy profile of the given energy source.
 * 
 * This function updates the historical availability profile of the energy source
 * and calculates its predictability based on variance in historical availability.
 * 
 * \param source Pointer to the EnergySource structure to update.
 */
void update_energy_profile(EnergySource *source) {
    // Calculate today's available hours
    uint8_t available_hours_today = source->duration_hours * source->occurrences_per_day;

    // Shift historical availability data to make room for today's data
    for (int i = HISTORY_DAYS - 1; i > 0; i--) {
        source->availability_history[i] = source->availability_history[i - 1];
    }
    source->availability_history[0] = available_hours_today;

    // Calculate predictability using variance in historical data
    float total = 0;
    float variance = 0;

    // Compute mean of historical availability
    for (int i = 0; i < HISTORY_DAYS; i++) {
        total += source->availability_history[i];
    }
    float mean = total / HISTORY_DAYS;

    // Compute variance of historical availability
    for (int i = 0; i < HISTORY_DAYS; i++) {
        variance += (source->availability_history[i] - mean) * (source->availability_history[i] - mean);
    }

    // Calculate predictability score (lower variance = higher predictability)
    source->predictability = 1.0f / (1.0f + (variance / HISTORY_DAYS));
}

/**
 * \brief Checks if the energy source is available at the given hour.
 * 
 * This function determines whether the energy source can supply energy at the given hour
 * based on its availability schedule and active periods.
 * 
 * \param source Pointer to the EnergySource structure to check.
 * \param hours Current simulated hour (0-23).
 * 
 * \return `true` if the energy source is available at the given hour; otherwise, `false`.
 */
bool is_energy_available(EnergySource *source, uint8_t hours) {
    // Calculate the interval between occurrences
    uint8_t interval_hours = 24 / source->occurrences_per_day;

    // Check each occurrence to see if the given hour falls within an active period
    for (uint8_t i = 0; i < source->occurrences_per_day; i++) {
        uint8_t start_time = (source->start_hour + i * interval_hours) % 24;
        uint8_t end_time = (start_time + source->duration_hours) % 24;

        if (start_time < end_time) {
            // Normal range without crossing midnight
            if (hours >= start_time && hours < end_time) {
                return true;
            }
        } else {
            // Range crosses midnight
            if (hours >= start_time || hours < end_time) {
                return true;
            }
        }
    }
    return false;
}
