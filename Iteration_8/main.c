#include "energy_manager.h"
#include "hardware_abstraction.h"
#include "task_manager.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MEASURES 10

static float temperature_values[NUM_MEASURES];
static uint8_t measure_index = 0;
static bool buffer_full = false;
static uint8_t simulated_hour = 0;
static uint8_t simulated_day = 0;
static uint8_t tensor_arena[2048];
static TinyMLModel ml_model;
static uint8_t global_taskcounter = 0;

static void incrementTaskCounter(const char* taskName) {
    global_taskcounter++;
    printf("%u tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}

static float compute_task_metric(const Task* task, const EnergySource* source, const GoalParameters* goal_params) {
    float critical_bonus = 0.0f;
    float weight_bonus = 0.0f;

    if (goal_params->goal == MAXIMIZE_RESILIENCE && task->critical) {
        critical_bonus = 2.0f;
    }

    if (task->weight > 0) {
        weight_bonus = 1.0f / (float)task->weight;
    }

    return (float)task->priority + critical_bonus + source->predictability + weight_bonus;
}

static int parse_duration_days(int argc, char* argv[], bool* infinite_loop) {
    int duration_days = 1;

    *infinite_loop = false;
    if (argc <= 1) {
        return duration_days;
    }

    duration_days = atoi(argv[1]);
    if (duration_days < 0) {
        printf("Invalid argument. Usage: %s [duration_days]\n", argv[0]);
        exit(1);
    }

    *infinite_loop = (duration_days == 0);
    return duration_days;
}

static float read_temperature(void) {
    const float min_temperature = -40.0f;
    const float max_temperature = 60.0f;
    float random_value = (float)rand() / (float)RAND_MAX;
    float temperature = min_temperature + random_value * (max_temperature - min_temperature);
    float rounded_temperature = ((int)(temperature * 10.0f)) / 10.0f;

    printf("Read a temperature of %.1f C\n", rounded_temperature);
    return rounded_temperature;
}

static float average_temperature(void) {
    float sum = 0.0f;
    uint8_t count = buffer_full ? NUM_MEASURES : measure_index;

    for (uint8_t i = 0; i < count; i++) {
        sum += temperature_values[i];
    }

    return count > 0 ? (sum / count) : 0.0f;
}

static void runTempTask(void) {
    temperature_values[measure_index] = read_temperature();
    measure_index = (measure_index + 1) % NUM_MEASURES;
    if (measure_index == 0) {
        buffer_full = true;
    }

    incrementTaskCounter("runTempTask");
}

static void computeAvgTempTask(void) {
    if (!buffer_full) {
        printf("Skipping average computation until the sample buffer is full.\n");
        return;
    }

    printf("Average of collected temperatures: %.2f C\n", average_temperature());
    incrementTaskCounter("computeAvgTempTask");
}

static void sendResultTask(void) {
    if (!buffer_full) {
        printf("Skipping data transmission until the sample buffer is full.\n");
        return;
    }

    blink_led(NUM_MEASURES);
    buffer_full = false;
    measure_index = 0;
    incrementTaskCounter("sendResultTask");
}

void update_simulated_hour(void) {
    simulated_hour = (simulated_hour + 1) % 24;
    if (simulated_hour == 0) {
        simulated_day++;
    }
}

uint8_t get_current_hour(void) {
    return simulated_hour;
}

static void enqueue_simulation_tasks(TaskQueue* queue, Task tasks[], size_t task_count) {
    for (size_t i = 0; i < task_count; i++) {
        tasks[i].completed = false;
        enqueue_task(queue, &tasks[i]);
    }
}

int main(int argc, char* argv[]) {
    bool infinite_loop = false;
    int duration_days = 0;
    float input_data[] = {1.0f, 0.5f};
    TaskQueue* queue = NULL;
    Task runTempTaskStruct = {runTempTask, 5000, 2, 3, false, 0.0f, false, NULL, 0};
    Task computeAvgTempTaskStruct = {computeAvgTempTask, 5000, 1, 2, true, 0.0f, false, NULL, 0};
    Task sendResultTaskStruct = {sendResultTask, 1000, 3, 1, true, 0.0f, false, NULL, 0};
    EnergySource energy_source = {WIND, 6, 3, 3, {0}, 0.0f};
    GoalParameters goal_params;
    Task simulation_tasks[] = {runTempTaskStruct, computeAvgTempTaskStruct, sendResultTaskStruct};
    Task* compute_dependencies_runtime[] = {&simulation_tasks[0]};
    Task* send_dependencies_runtime[] = {&simulation_tasks[1]};
    size_t simulation_task_count = sizeof(simulation_tasks) / sizeof(simulation_tasks[0]);

    srand((unsigned int)time(NULL));

    ml_model.tensor_arena = tensor_arena;
    ml_model.tensor_arena_size = sizeof(tensor_arena);
    printf("Predicted energy availability: %.2f\n",
           predict_energy(&ml_model, input_data, sizeof(input_data) / sizeof(input_data[0])));

    init_peripherals();
    queue = init_task_queue(MAX_TASKS);
    if (queue == NULL) {
        return 1;
    }

    duration_days = parse_duration_days(argc, argv, &infinite_loop);
    goal_params.goal = MAXIMIZE_RESILIENCE;
    goal_params.duration_days = (uint8_t)duration_days;

    simulation_tasks[1].dependencies = compute_dependencies_runtime;
    simulation_tasks[1].num_dependencies = 1;
    simulation_tasks[2].dependencies = send_dependencies_runtime;
    simulation_tasks[2].num_dependencies = 1;

    while (infinite_loop || simulated_day < goal_params.duration_days) {
        if (!is_energy_available(&energy_source)) {
            enter_deep_sleep(5000);
            update_simulated_hour();
            continue;
        }

        update_energy_profile(&energy_source);
        for (size_t i = 0; i < simulation_task_count; i++) {
            simulation_tasks[i].heuristic_metric =
                compute_task_metric(&simulation_tasks[i], &energy_source, &goal_params);
        }

        enqueue_simulation_tasks(queue, simulation_tasks, simulation_task_count);
        execute_tasks(queue, &energy_source, &goal_params);
        update_simulated_hour();
    }

    free_task_queue(queue);
    return 0;
}
