#include "task_manager.h"
#include "energy_manager.h"
#include "hardware_abstraction.h"
//#include "tensorflow/lite/micro/micro_error_reporter.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define NUM_MEASURES 10


/////////////////////
/// Global variables
/////////////////////


float temperature_values[NUM_MEASURES];
uint8_t measure_index = 0;
bool buffer_full = false;
uint8_t simulated_hour = 0;   // Heure simulée pour la source d'énergie
uint8_t simulated_day = 0;    // Jour simulé pour la durée de simulation


uint8_t tensor_arena[2048];
TinyMLModel ml_model;

//const uint8_t model_data[] = {/* Your TinyML model data */};


// Variable globale pour stocker les paramètres de l'objectif
GoalParameters global_goal_params;



/////////////////////
/// Implementation of Tasks
/////////////////////

// Fonction pour lire la température
float read_temperature() {
    // Set the range
    float min = -40.0f;
    float max = 60.0f;

    float temp = min + ((float)rand() / (float)RAND_MAX) * (max - min);
    float temp_1d = ((int)(temp * 10)) / 10.0f;
    printf("Read a temperature of %.f\n", temp_1d);
    return temp_1d;
}

// Fonction pour calculer la moyenne des températures
float average() {
    float sum = 0.0;
    uint8_t count = buffer_full ? NUM_MEASURES : measure_index;
    for (uint8_t i = 0; i < count; i++) {
        sum += temperature_values[i];
    }
    return (count > 0) ? (sum / count) : 0;
}

// Task - Read Temperature sensor
void runTempTask() {
    temperature_values[measure_index] = read_temperature();
    measure_index = (measure_index + 1) % NUM_MEASURES;
    if (measure_index == 0) buffer_full = true;
}

// Task - Compute the average of the Temperature collected
void computeAvgTempTask() {
    if (buffer_full) {  // Calcul seulement quand le buffer est plein
        float average_temp = average();
        printf("Average of collected temperatures: %.2f C\n", average_temp);
    }
}

// Task - Send the computed data to a received // This feature is simulated by a led blinking.
void sendResultTask() {
    if (buffer_full) {  // Clignote seulement quand le buffer est plein
        blink_led(NUM_MEASURES);
        buffer_full = false;
        measure_index = 0;
    }
}

// Fonction pour forcer l'update de l'heure simulée
void update_simulated_hour() {
    simulated_hour = (simulated_hour + 1) % 24;
    if (simulated_hour == 0) {
        simulated_day++;
    }
}


// Fonction pour obtenir l'heure simulée
uint8_t get_current_hour() {
    return simulated_hour;
}



int main() {
	
    // Load the TinyML model
    // if (!load_tinyml_model(&ml_model, model_data, sizeof(model_data), tensor_arena, sizeof(tensor_arena))) {
    //     printf("Failed to load TinyML model.\n");
    //     return 1;
    // }

    // Example of using TinyML to predict energy availability
    float input_data[] = {1.0, 0.5};  // Example input
    float predicted_energy = predict_energy(&ml_model, input_data, sizeof(input_data) / sizeof(float));

    printf("Predicted energy availability: %.2f\n", predicted_energy);


    // Initialisation des périphériques et de la file d'attente
    init_peripherals();
    TaskQueue* queue = init_task_queue(MAX_TASKS);

    // Initialisation des tâches
    Task runTempTaskStruct = { runTempTask, 5000, 2, 3, false, 0.0, NULL, 0 };
    Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 1, 2, true, 0.0, NULL, 0 };
    Task sendResultTaskStruct = { sendResultTask, 1000, 3, 1, true, 0.0, NULL, 0 };

    // Initialisation des dépendances
    sendResultTaskStruct.dependencies = &computeAvgTempTaskStruct;
    sendResultTaskStruct.num_dependencies = 1;

    // Initialisation de la source d'énergie et des paramètres
    EnergySource energy_source = { WIND, 6, 3, 3, {0}, 0.0 };
    GoalParameters goal_params = { MAXIMIZE_RESILIENCE, 5 };

    // Simulation principale
    while (simulated_day < goal_params.duration_days) {
        if (is_energy_available(&energy_source)) {
            update_energy_profile(&energy_source);
            execute_tasks(queue, &energy_source, &goal_params);
        } else {
            enter_deep_sleep(5000);
        }
        update_simulated_hour();
    }

    // Libération de la file d'attente
    free_task_queue(queue);
    return 0;
}
