#include "task_manager.h"
#include "hardware_abstraction.h"
#include "error_handling.h"
#include <stdlib.h>

/**
 * \brief Initializes a task queue with a specified capacity.
 * 
 * Allocates memory for a task queue and its internal array of tasks.
 * 
 * \param capacity The maximum number of tasks the queue can hold.
 * \return A pointer to the initialized task queue.
 */
TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

/**
 * \brief Adds a task to the task queue.
 * 
 * Enqueues a task into the queue while maintaining the heap property
 * based on the heuristic metric of the tasks.
 * 
 * \param queue A pointer to the task queue.
 * \param task A pointer to the task to be added to the queue.
 * 
 * \note If the queue is full, an error is logged.
 */
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

/**
 * \brief Removes and returns the highest-priority task from the queue.
 * 
 * Dequeues the task with the highest heuristic metric from the queue.
 * Maintains the heap property after removal.
 * 
 * \param queue A pointer to the task queue.
 * \return A pointer to the highest-priority task, or NULL if the queue is empty.
 * 
 * \note If the queue is empty, an error is logged.
 */
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

/**
 * \brief Frees memory allocated for the task queue.
 * 
 * Releases memory for the internal task array and the queue itself.
 * 
 * \param queue A pointer to the task queue to be freed.
 */
void free_task_queue(TaskQueue* queue) {
    free(queue->tasks);
    free(queue);
}

/**
 * \brief Executes tasks from the queue based on their dependencies and metrics.
 * 
 * Processes tasks in the queue while checking their dependencies.
 * Executes each task if its dependencies are met.
 * 
 * \param queue A pointer to the task queue.
 * \param source A pointer to the energy source used for task scheduling.
 * \param goal_params A pointer to the goal parameters for task execution.
 */
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
