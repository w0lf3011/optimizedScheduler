#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "energy_manager.h"

/// \brief Maximum number of tasks in the queue.
#define MAX_TASKS 10

/**
 * \brief Represents a task with various attributes for scheduling.
 */
typedef struct Task {
    void (*taskFunction)();      ///< Pointer to the function implementing the task.
    uint32_t delay_ms;           ///< Delay in milliseconds before executing the task.
    uint8_t priority;            ///< Priority of the task (higher value = higher priority).
    uint8_t weight;              ///< Weight representing the execution cost or duration.
    bool critical;               ///< Indicates if the task is critical for resilience.
    float heuristic_metric;      ///< Heuristic metric for task scheduling decisions.
    struct Task** dependencies;  ///< Array of pointers to dependent tasks.
    uint8_t num_dependencies;    ///< Number of dependencies for the task.
} Task;

/**
 * \brief Represents a priority-based task queue.
 */
typedef struct TaskQueue {
    Task** tasks;                ///< Array of pointers to tasks in the queue.
    uint8_t capacity;            ///< Maximum capacity of the queue.
    uint8_t size;                ///< Current number of tasks in the queue.
} TaskQueue;

/**
 * \brief Defines the objectives for task scheduling.
 */
typedef enum {
    MAXIMIZE_TASKS,              ///< Maximize the number of tasks executed.
    MAXIMIZE_RESILIENCE          ///< Maximize resilience by prioritizing critical tasks.
} GoalType;

/**
 * \brief Parameters defining the scheduling goal and simulation duration.
 */
typedef struct {
    GoalType goal;               ///< Selected scheduling goal.
    uint8_t duration_days;       ///< Duration of the simulation in days.
} GoalParameters;

/**
 * \brief Initializes a task queue.
 * 
 * Allocates memory for a task queue and its internal array of tasks.
 * 
 * \param capacity Maximum number of tasks the queue can hold.
 * \return Pointer to the initialized task queue.
 */
TaskQueue* init_task_queue(uint8_t capacity);

/**
 * \brief Adds a task to the task queue.
 * 
 * Inserts a task into the queue while maintaining the priority order
 * based on the heuristic metric.
 * 
 * \param queue Pointer to the task queue.
 * \param task Pointer to the task to be added to the queue.
 * 
 * \note Logs an error if the queue is full.
 */
void enqueue_task(TaskQueue* queue, Task* task);

/**
 * \brief Removes and returns the highest-priority task from the queue.
 * 
 * Dequeues the task with the highest heuristic metric from the queue.
 * 
 * \param queue Pointer to the task queue.
 * \return Pointer to the highest-priority task, or NULL if the queue is empty.
 * 
 * \note Logs an error if the queue is empty.
 */
Task* dequeue_task(TaskQueue* queue);

/**
 * \brief Frees the memory allocated for the task queue.
 * 
 * Releases memory for the internal task array and the task queue itself.
 * 
 * \param queue Pointer to the task queue to be freed.
 */
void free_task_queue(TaskQueue* queue);

/**
 * \brief Executes tasks in the queue based on their dependencies and scheduling metrics.
 * 
 * Processes tasks in the queue while checking and validating their dependencies.
 * 
 * \param queue Pointer to the task queue.
 * \param source Pointer to the energy source used for task scheduling.
 * \param goal_params Pointer to the goal parameters for task execution.
 */
void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params);

#endif // TASK_MANAGER_H
