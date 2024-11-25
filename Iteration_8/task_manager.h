#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "energy_manager.h"

#define MAX_TASKS 10

typedef struct Task {
    void (*taskFunction)();
    uint32_t delay_ms;
    uint8_t priority;
    uint8_t weight;
    bool critical;
    float heuristic_metric;
    struct Task** dependencies;
    uint8_t num_dependencies;
} Task;

typedef struct TaskQueue {
    Task** tasks;
    uint8_t capacity;
    uint8_t size;
} TaskQueue;

// Enumération des objectifs possibles
typedef enum {
    MAXIMIZE_TASKS,
    MAXIMIZE_RESILIENCE
} GoalType;

// Structure des paramètres de l'objectif
typedef struct {
    GoalType goal;               // Objectif sélectionné
    uint8_t duration_days;       // Durée de la simulation en jours
} GoalParameters;

TaskQueue* init_task_queue(uint8_t capacity);
void enqueue_task(TaskQueue* queue, Task* task);
Task* dequeue_task(TaskQueue* queue);
void free_task_queue(TaskQueue* queue);
void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params);

#endif
