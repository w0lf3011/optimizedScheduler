#include "task_manager.h"
#include "hardware_abstraction.h"
#include "error_handling.h"
#include <stdlib.h>

TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

void enqueue_task(TaskQueue* queue, Task* task) {
    if (queue->size < queue->capacity) {
        queue->tasks[queue->size++] = task;
        for (int i = queue->size - 1; i > 0; i--) {
            if (queue->tasks[i]->heuristic_metric > queue->tasks[i - 1]->heuristic_metric) {
                Task* temp = queue->tasks[i];
                queue->tasks[i] = queue->tasks[i - 1];
                queue->tasks[i - 1] = temp;
            } else {
                break;
            }
        }
    } else {
        handle_error("Task queue is full");
    }
}

Task* dequeue_task(TaskQueue* queue) {
    if (queue->size == 0) {
        handle_error("Task queue is empty");
        return NULL;
    }
    return queue->tasks[--queue->size];
}

void free_task_queue(TaskQueue* queue) {
    free(queue->tasks);
    free(queue);
}

void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params) {
    for (int i = 0; i < queue->size; i++) {
        Task* task = queue->tasks[i];
        if (task->num_dependencies > 0) {
            for (int j = 0; j < task->num_dependencies; j++) {
                if (task->dependencies[j] != NULL) {
                    task->taskFunction();
                    delay(task->delay_ms);
                } else {
                    handle_error("Dependency error");
                }
            }
        } else {
            task->taskFunction();
            delay(task->delay_ms);
        }
    }
}
