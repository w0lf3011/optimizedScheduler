#ifndef ENERGY_MANAGER_H
#define ENERGY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
//#include "tensorflow/lite/micro/micro_interpreter.h"

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

// Example of TinyML Structure based on TensorFlow
typedef struct {
    // const tflite::Model* model;
    // tflite::MicroInterpreter* interpreter;
    // tflite::MicroAllocator* allocator;
    uint8_t* tensor_arena;
    size_t tensor_arena_size;
} TinyMLModel;


bool load_tinyml_model(TinyMLModel* ml_model, const uint8_t* model_data, size_t model_size, uint8_t* tensor_arena, size_t tensor_arena_size);
float predict_energy(TinyMLModel* ml_model, const float* input, size_t input_length);

void update_energy_profile(EnergySource *source);
bool is_energy_available(EnergySource *source);

void integrate_real_energy_profiler();

#endif
