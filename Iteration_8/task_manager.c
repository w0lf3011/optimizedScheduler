#include "task_manager.h"

#include <stddef.h>
#include <stdlib.h>

#include "error_handling.h"
#include "hardware_abstraction.h"

static void swap_tasks(Task** left, Task** right) {
    Task* temp = *left;
    *left = *right;
    *right = temp;
}

static bool are_dependencies_met(const Task* task) {
    for (uint8_t i = 0; i < task->num_dependencies; i++) {
        if (task->dependencies[i] == NULL || !task->dependencies[i]->completed) {
            return false;
        }
    }

    return true;
}

TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = NULL;

    if (capacity == 0) {
        handle_error("Task queue capacity must be greater than zero");
        return NULL;
    }

    queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    if (queue == NULL) {
        handle_error("Failed to allocate task queue");
        return NULL;
    }

    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    if (queue->tasks == NULL) {
        free(queue);
        handle_error("Failed to allocate task storage");
        return NULL;
    }

    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

void enqueue_task(TaskQueue* queue, Task* task) {
    if (queue == NULL || task == NULL) {
        handle_error("Cannot enqueue a NULL queue or task");
        return;
    }

    if (queue->size >= queue->capacity) {
        handle_error("Task queue is full");
        return;
    }

    queue->tasks[queue->size] = task;
    queue->size++;

    for (uint8_t index = queue->size - 1; index > 0;) {
        uint8_t parent = (index - 1) / 2;
        if (queue->tasks[index]->heuristic_metric <= queue->tasks[parent]->heuristic_metric) {
            break;
        }

        swap_tasks(&queue->tasks[index], &queue->tasks[parent]);
        index = parent;
    }
}

Task* dequeue_task(TaskQueue* queue) {
    Task* highest_priority_task = NULL;

    if (queue == NULL || queue->size == 0) {
        handle_error("Task queue is empty");
        return NULL;
    }

    highest_priority_task = queue->tasks[0];
    queue->size--;
    if (queue->size == 0) {
        return highest_priority_task;
    }

    queue->tasks[0] = queue->tasks[queue->size];

    for (uint8_t index = 0;;) {
        uint8_t left = (uint8_t)(2 * index + 1);
        uint8_t right = (uint8_t)(2 * index + 2);
        uint8_t largest = index;

        if (left < queue->size &&
            queue->tasks[left]->heuristic_metric > queue->tasks[largest]->heuristic_metric) {
            largest = left;
        }

        if (right < queue->size &&
            queue->tasks[right]->heuristic_metric > queue->tasks[largest]->heuristic_metric) {
            largest = right;
        }

        if (largest == index) {
            break;
        }

        swap_tasks(&queue->tasks[index], &queue->tasks[largest]);
        index = largest;
    }

    return highest_priority_task;
}

void free_task_queue(TaskQueue* queue) {
    if (queue == NULL) {
        return;
    }

    free(queue->tasks);
    free(queue);
}

void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params) {
    (void)source;
    (void)goal_params;

    if (queue == NULL) {
        handle_error("Cannot execute tasks from a NULL queue");
        return;
    }

    while (queue->size > 0) {
        Task* deferred[MAX_TASKS];
        uint8_t deferred_count = 0;
        bool progress_made = false;

        while (queue->size > 0) {
            Task* task = dequeue_task(queue);
            if (task == NULL) {
                continue;
            }

            if (!are_dependencies_met(task)) {
                if (deferred_count >= MAX_TASKS) {
                    handle_error("Too many deferred tasks while resolving dependencies");
                    return;
                }

                deferred[deferred_count] = task;
                deferred_count++;
                continue;
            }

            task->taskFunction();
            task->completed = true;
            delay(task->delay_ms);
            progress_made = true;
        }

        for (uint8_t i = 0; i < deferred_count; i++) {
            enqueue_task(queue, deferred[i]);
        }

        if (!progress_made) {
            handle_error("Dependency cycle detected or no executable task available");
            return;
        }
    }
}
