#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#define NUM_MEASURES 10       // Nombre de mesures stockées
#define LED_PIN 25            // Pin GPIO pour la LED
#define TEMP_SENSOR_ADDR 0x48 // Adresse I2C du capteur de température (exemple)
#define PRIORITY_HIGH = 3
#define PRIORITY_MEDIUM = 2
#define PRIORITY_LOW = 1


// Structure de la tâche avec priorité
typedef struct {
    void (*taskFunction)();  // Pointeur de fonction pour la tâche
    uint32_t delay_ms;       // Délai avant l’exécution de la tâche en ms
    uint8_t priority;        // Priorité de la tâche
} Task;

float temperature_values[NUM_MEASURES];
uint8_t measure_index = 0;
bool buffer_full = false; // Indique si le buffer circulaire est plein

// Initialisation des périphériques
void init_peripherals() {
    printf("Initializing peripherals (simulated for Linux)...\n");
}

// Fonction pour lire la température
float read_temperature() {
    // Set the range
    float min = -40.0f;
    float max = 60.0f;

    float temp = min + ((float)rand() / (float)RAND_MAX) * (max - min);
    float temp_1d = ((int)(temp * 10)) / 10.0f;
    printf("Read a temperature of %.f\n", temp_1d);
    return temp_1d;
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
    printf("Simulating blinking LED %d times...\n", times);
    for (uint8_t i = 0; i < times; i++) {
        printf("LED ON\n");
        usleep(200000);  // 200 ms
        printf("LED OFF\n");
        usleep(200000);
    }
}

// Fonction pour simuler un sleep
void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);  // Convertit ms en µs
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

// Fonction de comparaison pour trier les tâches par priorité (plus élevé en premier)
int compare_tasks(const void* a, const void* b) {
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    return (taskB->priority - taskA->priority); // Ordre décroissant de priorité
}

// Fonction d'exécution de toutes les tâches
void task_scheduler(Task* tasks, uint8_t task_count) {
    // Trier les tâches par priorité
    qsort(tasks, task_count, sizeof(Task), compare_tasks);
    
    // Exécuter chaque tâche dans l'ordre de priorité
    for (uint8_t i = 0; i < task_count; i++) {
        tasks[i].taskFunction();     // Exécuter la tâche
        sleep_ms(tasks[i].delay_ms); // Délai après chaque tâche
    }
}

int main() {
    init_peripherals();

    // Création des tâches avec leurs fonctions, délais et priorités
    Task tasks[] = {
        { runTempTask, 5000, 1 },         // Priorité basse
        { computeAvgTempTask, 5000, 2 },  // Priorité moyenne
        { sendResultTask, 1000, 3 }       // Priorité haute
    };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    // Boucle principale
    int counter = 10;
    while (true) {
        task_scheduler(tasks, task_count);  // Exécuter les tâches
        sleep_ms(1000);
        counter--;
    }
}
