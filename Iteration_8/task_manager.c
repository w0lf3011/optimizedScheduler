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
        
        // Heapify up to maintain heap property
        int i = queue->size - 1;
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (queue->tasks[i]->heuristic_metric > queue->tasks[parent]->heuristic_metric) {
                Task* temp = queue->tasks[i];
                queue->tasks[i] = queue->tasks[parent];
                queue->tasks[parent] = temp;
                i = parent;
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

    // Heapify down to maintain heap property
    int i = 0;
    while (i < queue->size) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < queue->size && queue->tasks[left]->heuristic_metric > queue->tasks[largest]->heuristic_metric) {
            largest = left;
        }
        if (right < queue->size && queue->tasks[right]->heuristic_metric > queue->tasks[largest]->heuristic_metric) {
            largest = right;
        }
        if (largest != i) {
            Task* temp = queue->tasks[i];
            queue->tasks[i] = queue->tasks[largest];
            queue->tasks[largest] = temp;
            i = largest;
        } else {
            break;
        }
    }

    return queue->tasks[--queue->size];
}

void free_task_queue(TaskQueue* queue) {
    free(queue->tasks);
    free(queue);
}

void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params) {
    while (queue->size > 0) {
        Task* task = dequeue_task(queue);

        // Validate dependencies
        bool dependencies_met = true;
        for (int j = 0; j < task->num_dependencies; j++) {
            if (task->dependencies[j] != NULL) {
                dependencies_met = false;
                handle_error("Dependency error: Unmet dependencies.");
                break;
            }
        }

        if (dependencies_met) {
            task->taskFunction();
            delay(task->delay_ms);
        }
    }
}
