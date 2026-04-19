#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "energy_manager.h"

/**
 * \brief Maximum number of tasks supported by the demo scheduler queue.
 */
#define MAX_TASKS 10

/**
 * \brief Describes a schedulable unit of work.
 *
 * Each task exposes an execution function, timing and scheduling metadata,
 * and an optional dependency list used by the scheduler before execution.
 */
typedef struct Task {
    void (*taskFunction)();      ///< Function executed when the task is scheduled.
    uint32_t delay_ms;           ///< Delay applied after task execution.
    uint8_t priority;            ///< Base priority used by the heuristic scheduler.
    uint8_t weight;              ///< Relative execution cost used in the heuristic score.
    bool critical;               ///< Whether the task is important for resilience-oriented scheduling.
    float heuristic_metric;      ///< Computed score used by the priority queue.
    bool completed;              ///< Completion marker for the current scheduling cycle.
    struct Task** dependencies;  ///< Tasks that must complete before this task may run.
    uint8_t num_dependencies;    ///< Number of task dependencies.
} Task;

/**
 * \brief Heap-backed priority queue of tasks.
 */
typedef struct TaskQueue {
    Task** tasks;      ///< Heap storage for queued tasks.
    uint8_t capacity;  ///< Maximum number of tasks accepted by the queue.
    uint8_t size;      ///< Current number of queued tasks.
} TaskQueue;

/**
 * \brief High-level optimization goal for the scheduler.
 */
typedef enum {
    MAXIMIZE_TASKS,       ///< Favor overall throughput.
    MAXIMIZE_RESILIENCE   ///< Favor critical tasks and system continuity.
} GoalType;

/**
 * \brief Runtime parameters for a simulation session.
 */
typedef struct {
    GoalType goal;        ///< Scheduler optimization objective.
    uint8_t duration_days;///< Number of simulated days to execute.
} GoalParameters;

/**
 * \brief Allocates and initializes a task queue.
 *
 * \param capacity Maximum number of tasks supported by the queue.
 * \return Pointer to the queue, or NULL if allocation fails.
 */
TaskQueue* init_task_queue(uint8_t capacity);

/**
 * \brief Inserts a task into the priority queue.
 *
 * Tasks are ordered by their heuristic metric, highest score first.
 *
 * \param queue Target queue.
 * \param task Task to insert.
 */
void enqueue_task(TaskQueue* queue, Task* task);

/**
 * \brief Removes the task with the highest heuristic metric.
 *
 * \param queue Source queue.
 * \return Highest-priority task, or NULL if the queue is empty.
 */
Task* dequeue_task(TaskQueue* queue);

/**
 * \brief Releases all memory allocated for a task queue.
 *
 * \param queue Queue to free.
 */
void free_task_queue(TaskQueue* queue);

/**
 * \brief Executes queued tasks while enforcing their dependencies.
 *
 * Tasks whose dependencies are not yet completed are deferred and retried
 * within the current scheduling cycle. A scheduling error is reported if no
 * progress can be made.
 *
 * \param queue Queue of tasks to execute.
 * \param source Energy source associated with the current cycle.
 * \param goal_params Goal configuration for the running simulation.
 */
void execute_tasks(TaskQueue* queue, EnergySource* source, GoalParameters* goal_params);

#endif
