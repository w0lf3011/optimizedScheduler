#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
//#include "tensorflow/lite/micro/micro_interpreter.h"

/**
 * \brief Enumerates types of energy sources.
 */
typedef enum {
    SOLAR,   ///< Solar energy source.
    WIND,    ///< Wind energy source.
    BATTERY  ///< Battery energy source.
} EnergyType;

/**
 * \brief Represents an energy source with its properties and historical availability.
 */
typedef struct {
    EnergyType type;              ///< Type of energy source.
    uint8_t start_hour;           ///< Hour of the first occurrence (0-23).
    uint8_t duration_hours;       ///< Duration of each active period in hours.
    uint8_t occurrences_per_day;  ///< Number of active periods per day.
    uint8_t availability_history[5]; ///< Historical data of availability over 5 days (in hours).
    float predictability;         ///< Predictability score based on historical data.
} EnergySource;

/**
 * \brief Represents a TinyML model for energy prediction.
 */
typedef struct {
    // const tflite::Model* model;             ///< Pointer to the loaded TensorFlow Lite model.
    // tflite::MicroInterpreter* interpreter; ///< Pointer to the TinyML interpreter.
    // tflite::MicroAllocator* allocator;     ///< Pointer to the TinyML allocator.
    uint8_t* tensor_arena;          ///< Pointer to the tensor arena memory buffer.
    size_t tensor_arena_size;       ///< Size of the tensor arena in bytes.
} TinyMLModel;

/**
 * \brief Loads a TinyML model into memory and prepares it for inference.
 * 
 * \param ml_model Pointer to a TinyMLModel structure to store the loaded model.
 * \param model_data Pointer to the raw model data.
 * \param model_size Size of the model data in bytes.
 * \param tensor_arena Pointer to a memory buffer for tensors.
 * \param tensor_arena_size Size of the tensor memory buffer in bytes.
 * 
 * \return True if the model is loaded successfully, false otherwise.
 */
bool load_tinyml_model(TinyMLModel* ml_model, const uint8_t* model_data, size_t model_size, uint8_t* tensor_arena, size_t tensor_arena_size);

/**
 * \brief Uses a TinyML model to predict energy availability based on input data.
 * 
 * \param ml_model Pointer to the TinyMLModel structure containing the loaded model.
 * \param input Pointer to the input data array.
 * \param input_length Length of the input data array.
 * 
 * \return Predicted energy availability as a float value.
 */
float predict_energy(TinyMLModel* ml_model, const float* input, size_t input_length);

/**
 * \brief Updates the energy source profile by recording historical data and calculating predictability.
 * 
 * This function shifts historical availability data, calculates the mean and variance of availability,
 * and updates the predictability score of the energy source.
 * 
 * \param source Pointer to the EnergySource structure to update.
 */
void update_energy_profile(EnergySource *source);

/**
 * \brief Checks if energy is currently available based on the energy source's profile.
 * 
 * This function determines availability by evaluating the energy source's scheduled availability
 * within the simulated current hour.
 * 
 * \param source Pointer to the EnergySource structure to evaluate.
 * 
 * \return True if energy is available, false otherwise.
 */
bool is_energy_available(EnergySource *source);

/**
 * \brief Integrates real-world energy profiling tools.
 * 
 * Placeholder function for integrating actual hardware or tools for profiling energy availability.
 */
void integrate_real_energy_profiler();

#endif // ENERGY_MANAGER_H
