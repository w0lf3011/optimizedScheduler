#include "task_manager.h"
#include "hardware_abstraction.h"
#include "error_handling.h"
#include <stdlib.h>

/**
 * \brief Initializes a task queue with the specified capacity.
 * 
 * Allocates memory for the task queue and its internal task array.
 * 
 * \param capacity Maximum number of tasks the queue can hold.
 * \return Pointer to the initialized TaskQueue structure.
 */
TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

/**
 * \brief Adds a task to the task queue, maintaining the order based on heuristic metrics.
 * 
 * Tasks are inserted into the queue in descending order of their heuristic metrics.
 * If the queue is full, an error is handled via `handle_error`.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \param task Pointer to the Task to be added.
 */
void enqueue_task(TaskQueue* queue, Task* task) {
    if (queue->size < queue->capacity) {
        queue->tasks[queue->size++] = task;
        // Sort tasks based on heuristic metrics
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

/**
 * \brief Removes and returns the most prioritized task from the queue.
 * 
 * If the queue is empty, an error is handled via `handle_error`.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \return Pointer to the dequeued Task, or NULL if the queue is empty.
 */
Task* dequeue_task(TaskQueue* queue) {
    if (queue->size == 0) {
        handle_error("Task queue is empty");
        return NULL;
    }
    return queue->tasks[--queue->size];
}

/**
 * \brief Frees the memory allocated for the task queue.
 * 
 * Deallocates the memory used by the queue and its internal task array.
 * 
 * \param queue Pointer to the TaskQueue structure to be freed.
 */
void free_task_queue(TaskQueue* queue) {
    free(queue->tasks);
    free(queue);
}

/**
 * \brief Executes tasks from the task queue based on their dependencies and priorities.
 * 
 * Iterates through the tasks in the queue and executes each task. Dependencies are checked,
 * and tasks are delayed according to their `delay_ms` property.
 * If any dependency is NULL, an error is handled via `handle_error`.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \param source Pointer to the EnergySource structure representing the energy source.
 * \param goal_params Pointer to the GoalParameters structure defining the simulation goals.
 */
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
