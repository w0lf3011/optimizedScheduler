#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "energy_manager.h"

/// \brief Maximum number of tasks that can be managed.
#define MAX_TASKS 10

/**
 * \brief Represents a task with its associated properties and dependencies.
 */
typedef struct Task {
    void (*taskFunction)();      ///< Pointer to the task function.
    uint32_t delay_ms;           ///< Delay in milliseconds before executing the task.
    uint8_t priority;            ///< Task priority (higher value = higher priority).
    uint8_t weight;              ///< Weight representing execution cost or duration.
    bool critical;               ///< Indicates if the task is critical for resilience.
    float heuristic_metric;      ///< Heuristic metric for task scheduling.
    struct Task** dependencies;  ///< Array of pointers to dependent tasks.
    uint8_t num_dependencies;    ///< Number of dependent tasks.
} Task;

/**
 * \brief Represents a queue for managing tasks.
 */
typedef struct TaskQueue {
    Task** tasks;      ///< Array of pointers to tasks.
    uint8_t capacity;  ///< Maximum capacity of the queue.
    uint8_t size;      ///< Current number of tasks in the queue.
} TaskQueue;

/**
 * \brief Enum representing possible simulation goals.
 */
typedef enum {
    MAXIMIZE_TASKS,      ///< Goal to maximize the number of executed tasks.
    MAXIMIZE_RESILIENCE  ///< Goal to maximize system resilience.
} GoalType;

/**
 * \brief Structure representing the parameters for the simulation goal.
 */
typedef struct {
    GoalType goal;        ///< Selected simulation goal.
    uint8_t duration_days; ///< Duration of the simulation in days.
} GoalParameters;

/**
 * \brief Initializes a task queue with the specified capacity.
 * 
 * \param capacity Maximum number of tasks the queue can hold.
 * \return Pointer to the initialized TaskQueue structure.
 */
TaskQueue* init_task_queue(uint8_t capacity);

/**
 * \brief Adds a task to the task queue, maintaining the order based on heuristic metrics.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \param task Pointer to the Task to be added.
 */
void enqueue_task(TaskQueue* queue, Task* task);

/**
 * \brief Removes and returns the most prioritized task from the queue.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \return Pointer to the dequeued Task, or NULL if the queue is empty.
 */
Task* dequeue_task(TaskQueue* queue);

/**
 * \brief Frees the memory allocated for the task queue.
 * 
 * \param queue Pointer to the TaskQueue structure to be freed.
 */
void free_task_queue(TaskQueue* queue);

/**
 * \brief Executes tasks from the task queue based on the energy source and simulation goals.
 * 
 * \param queue Pointer to the TaskQueue structure.
 * \param source Pointer to the EnergySource structure representing the energy source.
 * \param goal_params Pointer to the GoalParameters structure defining the simulation goals.
 */
void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params);

#endif // TASK_MANAGER_H