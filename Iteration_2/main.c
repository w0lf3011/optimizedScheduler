#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_MEASURES 10       // Nombre de mesures stockées
#define LED_PIN 25            // Pin GPIO pour la LED
#define TEMP_SENSOR_ADDR 0x48 // Adresse I2C du capteur de température (exemple)

// Structure de la tâche avec priorité et poids
typedef struct Task {
    void (*taskFunction)();   // Pointeur de fonction pour la tâche
    uint32_t delay_ms;        // Délai avant l’exécution de la tâche en ms
    uint8_t priority;         // Priorité de la tâche (plus élevé = plus prioritaire)
    uint8_t weight;           // Poids représentant le coût ou la durée d’exécution
    struct Task** dependencies; // Pointeurs vers les tâches dépendantes
    uint8_t num_dependencies; // Nombre de dépendances
} Task;

float temperature_values[NUM_MEASURES];
uint8_t measure_index = 0;
bool buffer_full = false; // Indique si le buffer circulaire est plein

// Initialisation des périphériques
void init_peripherals() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
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

// Fonction pour trier les tâches en fonction du poids et de la priorité
int compare_tasks(const void* a, const void* b) {
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    // Tâches avec une priorité plus élevée ou un poids inférieur sont préférées
    if (taskA->priority != taskB->priority) {
        return taskB->priority - taskA->priority;
    }
    return taskA->weight - taskB->weight;
}

// Fonction pour exécuter les tâches dans l'ordre des priorités et dépendances
void execute_tasks(Task* tasks, uint8_t task_count) {
    // Tri des tâches par priorité et poids
    qsort(tasks, task_count, sizeof(Task), compare_tasks);

    // Exécuter les tâches en tenant compte des dépendances
    for (uint8_t i = 0; i < task_count; i++) {
        Task* task = &tasks[i];
        
        // Vérifier que toutes les dépendances sont terminées
        bool ready_to_run = true;
        for (uint8_t j = 0; j < task->num_dependencies; j++) {
            Task* dependency = task->dependencies[j];
            if (dependency) {
                ready_to_run = false;
                break;
            }
        }
        
        // Exécuter la tâche si elle est prête
        if (ready_to_run) {
            task->taskFunction();
            sleep_ms(task->delay_ms); // Temps de repos après exécution
        }
    }
}

int main() {
    init_peripherals();

    // Création des tâches avec leurs fonctions, délais, priorités et poids
    Task runTempTaskStruct = { runTempTask, 5000, 2, 3, NULL, 0 };
    Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 1, 2, NULL, 0 };
    Task sendResultTaskStruct = { sendResultTask, 1000, 3, 1, NULL, 0 };

    // Initialisation des dépendances
    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    // Ajouter des dépendances si nécessaire
    // Exemple : sendResultTask dépend de computeAvgTempTask
    sendResultTaskStruct.dependencies = &computeAvgTempTaskStruct;
    sendResultTaskStruct.num_dependencies = 1;

    // Boucle principale d'ordonnancement
    int counter = 10;
    while (counter > 0) {
        execute_tasks(tasks, task_count);
        sleep_ms(1000);
        counter--;
    }
}
