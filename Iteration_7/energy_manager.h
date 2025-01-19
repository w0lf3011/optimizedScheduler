#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief Types of energy sources.
 */
typedef enum {
    SOLAR,   ///< Solar energy.
    WIND,    ///< Wind energy.
    BATTERY  ///< Battery energy.
} EnergyType;

/**
 * \brief Structure defining an energy source with historical availability profile.
 */
typedef struct {
    EnergyType type;              ///< Type of energy source (e.g., solar, wind, or battery).
    uint8_t start_hour;           ///< Start hour of the first availability period (0-23).
    uint8_t duration_hours;       ///< Duration of each active period in hours.
    uint8_t occurrences_per_day;  ///< Number of availability periods per day.
    uint8_t availability_history[5]; ///< Historical availability in hours for the past 5 days.
    float predictability;         ///< Predictability score calculated from the historical profile.
} EnergySource;

/**
 * \brief Updates the energy profile of the given energy source.
 * 
 * This function updates the availability history of the energy source and calculates its 
 * predictability score based on variance in historical data.
 * 
 * \param source Pointer to the EnergySource structure to update.
 */
void update_energy_profile(EnergySource *source);

/**
 * \brief Checks if the energy source is available at the given hour.
 * 
 * This function determines whether the energy source can supply energy based on its 
 * availability schedule.
 * 
 * \param source Pointer to the EnergySource structure to check.
 * \param hours Current simulated hour (0-23).
 * 
 * \return `true` if the energy source is available; otherwise, `false`.
 */
bool is_energy_available(EnergySource *source, uint8_t hours);

#endif // ENERGY_MANAGER_H
