#include "energy_manager.h"
#include "hardware_abstraction.h"
//#include "tensorflow/lite/micro/all_ops_resolver.h"
//#include "tensorflow/lite/micro/micro_error_reporter.h"

#define HISTORY_DAYS 5 


// Example of code to load a TinyML Model
bool load_tinyml_model(TinyMLModel* ml_model, const uint8_t* model_data, size_t model_size, uint8_t* tensor_arena, size_t tensor_arena_size) {    
    // ml_model->model = tflite::GetModel(model_data);
    // if (ml_model->model->version() != TFLITE_SCHEMA_VERSION) {
    //     handle_error("Model schema version mismatch");
    //     return false;
    // }

    // ml_model->tensor_arena = tensor_arena;
    // ml_model->tensor_arena_size = tensor_arena_size;

    // static tflite::AllOpsResolver resolver;
    // static tflite::MicroErrorReporter error_reporter;
    // ml_model->interpreter = new tflite::MicroInterpreter(ml_model->model, resolver, tensor_arena, tensor_arena_size, &error_reporter);

    // if (ml_model->interpreter->AllocateTensors() != kTfLiteOk) {
    //     handle_error("Failed to allocate tensors");
    //     return false;
    // }

    return true;
}

float predict_energy(TinyMLModel* ml_model, const float* input, size_t input_length) {
    // TfLiteTensor* input_tensor = ml_model->interpreter->input(0);
    // if (input_tensor->dims->size != input_length) {
    //     handle_error("Input length mismatch");
    //     return -1.0f;
    // }

    // for (size_t i = 0; i < input_length; ++i) {
    //     input_tensor->data.f[i] = input[i];
    // }

    // if (ml_model->interpreter->Invoke() != kTfLiteOk) {
    //     handle_error("Model invocation failed");
    //     return -1.0f;
    // }

    // return ml_model->interpreter->output(0)->data.f[0];
    return 1.0f;
}


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
