#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_MEASURES 10         // Nombre de mesures stockées
#define LED_PIN 25              // Pin GPIO pour la LED
#define TEMP_SENSOR_ADDR 0x48   // Adresse I2C du capteur de température (exemple)
#define DEFAULT_SIMULATION_DAYS 7  // Durée par défaut de la simulation en jours
#define HISTORY_DAYS 5          // Nombre de jours d'historique pour le profil d'intermittence

// Types de sources d'énergie
typedef enum {
    SOLAR,
    WIND,
    BATTERY
} EnergyType;

// Enumération des objectifs possibles
typedef enum {
    MAXIMIZE_TASKS,
    MAXIMIZE_RESILIENCE
} GoalType;

// Structure pour définir une source d'énergie avec historique d'intermittence
typedef struct {
    EnergyType type;              // Type de source (solaire, éolienne, batterie)
    uint8_t start_hour;           // Heure de début de la première occurrence (0-23)
    uint8_t duration_hours;       // Durée de chaque période d'alimentation en heures
    uint8_t occurrences_per_day;  // Nombre de fois que la source est disponible par jour
    uint8_t availability_history[HISTORY_DAYS]; // Historique de disponibilité sur plusieurs jours (en heures)
    float predictability;         // Niveau de prévisibilité basé sur l'historique
} EnergySource;

// Structure des paramètres de l'objectif
typedef struct {
    GoalType goal;               // Objectif sélectionné
    uint8_t duration_days;       // Durée de la simulation en jours
} GoalParameters;

// Structure de la tâche avec métrique heuristique
typedef struct Task {
    void (*taskFunction)();      // Pointeur de fonction pour la tâche
    uint32_t delay_ms;           // Délai avant l’exécution de la tâche en ms
    uint8_t priority;            // Priorité de la tâche (plus élevé = plus prioritaire)
    uint8_t weight;              // Poids représentant le coût ou la durée d’exécution
    bool critical;               // Indique si la tâche est critique pour la résilience
    float heuristic_metric;      // Métrique heuristique pour la tâche
    struct Task** dependencies;  // Pointeurs vers les tâches dépendantes
    uint8_t num_dependencies;    // Nombre de dépendances
} Task;

// Structure de file d'attente pour gérer les tâches
typedef struct TaskQueue {
    Task** tasks;
    uint8_t capacity;
    uint8_t size;
} TaskQueue;

float temperature_values[NUM_MEASURES];
uint8_t measure_index = 0;
bool buffer_full = false;
uint8_t simulated_hour = 0;   // Heure simulée pour la source d'énergie
uint8_t simulated_day = 0;    // Jour simulé pour la durée de simulation

// Variable globale pour stocker les paramètres de l'objectif
GoalParameters global_goal_params;

// Initialisation des périphériques
void init_peripherals() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

// Initialisation de la file d'attente de tâches
TaskQueue* init_task_queue(uint8_t capacity) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    queue->tasks = (Task**)malloc(sizeof(Task*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    return queue;
}

// Ajout d'une tâche dans la file d'attente (triée par métrique heuristique)
void enqueue_task(TaskQueue* queue, Task* task) {
    if (queue->size < queue->capacity) {
        queue->tasks[queue->size++] = task;
        // Tri par insertion pour maintenir l'ordre des tâches selon leur métrique
        for (int i = queue->size - 1; i > 0; i--) {
            if (queue->tasks[i]->heuristic_metric > queue->tasks[i - 1]->heuristic_metric) {
                Task* temp = queue->tasks[i];
                queue->tasks[i] = queue->tasks[i - 1];
                queue->tasks[i - 1] = temp;
            } else {
                break;
            }
        }
    }
}

// Suppression et retour de la tâche la plus prioritaire
Task* dequeue_task(TaskQueue* queue) {
    if (queue->size == 0) return NULL;
    return queue->tasks[--queue->size];
}

// Fonction pour lire la température
float read_temperature() {
    float temp = 25.0;  // Exemple de température simulée
    return temp;
}

// Fonction pour calculer la moyenne des températures
float average() {
    float sum = 0.0;
    uint8_t count = buffer_full ? NUM_MEASURES : measure_index;
    for (uint8_t i = 0; i < count; i++) {
        sum += temperature_values[i];
    }
    return (count > 0) ? (sum / count) : 0;
}

// Fonction pour allumer la LED X fois
void blink_led(uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(200);
        gpio_put(LED_PIN, 0);
        sleep_ms(200);
    }
}

// Fonction de mise en sommeil profond pour économiser l'énergie
void deep_sleep(uint32_t duration_ms) {
    printf("Entering deep sleep for %u ms due to prolonged inactivity.\n", duration_ms);
    sleep_ms(duration_ms);  // Simule le mode de sommeil profond avec sleep_ms
}

// Tâche: Obtenir la température actuelle du capteur
void runTempTask() {
    temperature_values[measure_index] = read_temperature();
    measure_index = (measure_index + 1) % NUM_MEASURES;
    if (measure_index == 0) buffer_full = true;
}

// Tâche: Calculer la moyenne des températures collectées
void computeAvgTempTask() {
    if (buffer_full) {  // Calcul seulement quand le buffer est plein
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
    }
}

// Tâche: Envoyer le résultat, simulé par le clignotement de la LED
void sendResultTask() {
    if (buffer_full) {  // Clignote seulement quand le buffer est plein
        blink_led(NUM_MEASURES);
        buffer_full = false;
        measure_index = 0;
    }
}

// Fonction pour obtenir l'heure simulée
uint8_t get_current_hour() {
    return simulated_hour;
}

// Fonction pour forcer l'update de l'heure simulée
void update_simulated_hour() {
    simulated_hour = (simulated_hour + 1) % 24;
    if (simulated_hour == 0) {
        simulated_day++;
    }
}

// Fonction pour vérifier la disponibilité de la source d'énergie
bool is_energy_available(EnergySource *source) {
    uint8_t current_hour = get_current_hour();
    uint8_t interval_hours = 24 / source->occurrences_per_day;
    
    for (uint8_t i = 0; i < source->occurrences_per_day; i++) {
        uint8_t start_time = (source->start_hour + i * interval_hours) % 24;
        uint8_t end_time = (start_time + source->duration_hours) % 24;

        if (start_time < end_time) {
            if (current_hour >= start_time && current_hour < end_time) {
                return true;
            }
        } else {
            if (current_hour >= start_time || current_hour < end_time) {
                return true;
            }
        }
    }
    return false;
}

// Fonction pour mettre à jour le profil d'intermittence et calculer la prévisibilité
void update_energy_profile(EnergySource *source) {
    uint8_t available_hours_today = source->duration_hours * source->occurrences_per_day;
    for (int i = HISTORY_DAYS - 1; i > 0; i--) {
        source->availability_history[i] = source->availability_history[i - 1];
    }
    source->availability_history[0] = available_hours_today;

    // Calcul de la prévisibilité en utilisant l'historique
    float total = 0;
    float variance = 0;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        total += source->availability_history[i];
    }
    float mean = total / HISTORY_DAYS;
    for (int i = 0; i < HISTORY_DAYS; i++) {
        variance += (source->availability_history[i] - mean) * (source->availability_history[i] - mean);
    }
    source->predictability = 1.0f / (1.0f + (variance / HISTORY_DAYS));
}

// Fonction pour calculer la métrique heuristique de chaque tâche
void calculate_heuristic_metric(Task *task, GoalParameters *goal_params) {
    if (goal_params->goal == MAXIMIZE_TASKS) {
        task->heuristic_metric = task->priority * 0.5 + task->weight * 0.5;
    } else if (goal_params->goal == MAXIMIZE_RESILIENCE) {
        task->heuristic_metric = task->priority * 0.3 + task->weight * 0.2 + (task->critical ? 1.0 : 0.0) * 0.5;
    }
}

// Fonction pour trier les tâches en fonction de la métrique heuristique
int compare_tasks(const void* a, const void* b) {
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    if (taskA->heuristic_metric > taskB->heuristic_metric) return -1;
    if (taskA->heuristic_metric < taskB->heuristic_metric) return 1;
    return 0;
}

// Fonction pour exécuter les tâches en fonction de l'objectif sélectionné
void execute_tasks(Task* tasks, uint8_t task_count, EnergySource *source, GoalParameters *goal_params) {
    update_energy_profile(source);

    if (!is_energy_available(source)) {
        printf("Energy source is not available. Tasks are paused.\n");
        return;
    }

    // Calculer la métrique heuristique pour chaque tâche
    for (int i = 0; i < task_count; i++) {
        calculate_heuristic_metric(&tasks[i], goal_params);
    }

    // Tri des tâches en fonction de la métrique heuristique
    qsort(tasks, task_count, sizeof(Task), compare_tasks);

    for (uint8_t i = 0; i < task_count; i++) {
        Task* task = &tasks[i];

        bool ready_to_run = true;
        for (uint8_t j = 0; j < task->num_dependencies; j++) {
            Task* dependency = task->dependencies[j];
            if (dependency) {
                ready_to_run = false;
                break;
            }
        }

        if (ready_to_run) {
            task->taskFunction();
            sleep_ms(task->delay_ms);
        }
    }
}

// restent identiques.

int main() {
    init_peripherals();

    Task runTempTaskStruct = { runTempTask, 5000, 2, 3, false, 0.0, NULL, 0 };
    Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 1, 2, true, 0.0, NULL, 0 };
    Task sendResultTaskStruct = { sendResultTask, 1000, 3, 1, true, 0.0, NULL, 0 };

    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    sendResultTaskStruct.dependencies = &computeAvgTempTaskStruct;
    sendResultTaskStruct.num_dependencies = 1;

    EnergySource energy_source = { WIND, 6, 3, 3, {0}, 0.0 };
    GoalParameters goal_params = { MAXIMIZE_RESILIENCE, 5 };

    TaskQueue* queue = init_task_queue(task_count);

    // Simulation principale avec gestion dynamique des tâches et sommeil profond
    while (simulated_day < goal_params.duration_days) {
        if (is_energy_available(&energy_source)) {
            // Enqueue tasks based on updated metrics
            for (int i = 0; i < task_count; i++) {
                calculate_heuristic_metric(tasks[i], &goal_params);
                enqueue_task(queue, tasks[i]);
            }

            // Execute tasks from the queue
            Task* task;
            while ((task = dequeue_task(queue)) != NULL) {
                task->taskFunction();
                sleep_ms(task->delay_ms); // Délai après chaque tâche
            }
        } else {
            // Mode sommeil profond si l'énergie est indisponible
            deep_sleep(5000);  // Durée de sommeil profond (ajustable selon les besoins)
        }

        update_simulated_hour();
        sleep_ms(1000);  // Simulation d'un intervalle de contrôle
    }

    printf("Simulation completed over %d days.\n", goal_params.duration_days);
    free(queue->tasks);
    free(queue);
}
