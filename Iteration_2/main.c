#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

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
uint8_t global_taskcounter = 0;

// Increment Task Counter and show the task name.
void incrementTaskCounter(const char *taskName) {
    global_taskcounter++;
    printf("%d tasks executed so far. Last task: %s\n", global_taskcounter, taskName);
}


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
    incrementTaskCounter("runTempTask");
}

// Tâche: Calculer la moyenne des températures collectées
void computeAvgTempTask() {
    if (buffer_full) {  // Calcul seulement quand le buffer est plein
        float average_temp = average();
        printf("Average of collected temperatures: %.2f°C\n", average_temp);
        incrementTaskCounter("computeAvgTempTask");
    }
}

// Tâche: Envoyer le résultat, simulé par le clignotement de la LED
void sendResultTask() {
    if (buffer_full) {  // Clignote seulement quand le buffer est plein
        blink_led(NUM_MEASURES);
        buffer_full = false;
        measure_index = 0;
        incrementTaskCounter("sendResultTask");
    }
}

// Fonction pour trier les tâches en fonction du poids et de la priorité
int compare_tasks(const void* a, const void* b) {
    Task* taskA = *(Task**)a;
    Task* taskB = *(Task**)b;
    // Tâches avec une priorité plus élevée ou un poids inférieur sont préférées
    if (taskA->priority != taskB->priority) {
        return taskB->priority - taskA->priority;
    }
    return taskA->weight - taskB->weight;
}

// Fonction pour exécuter les tâches dans l'ordre des priorités et dépendances
void execute_tasks(Task* tasks[], uint8_t task_count) {
    // Tri des tâches par priorité et poids
    qsort(tasks, task_count, sizeof(Task*), compare_tasks);

    bool task_completed[task_count];
    for (uint8_t i = 0; i < task_count; i++) {
        task_completed[i] = false;
    }

    for (uint8_t i = 0; i < task_count; i++) {
        Task* task = tasks[i];
        bool ready_to_run = true;

        // Check Dependencies
        for (uint8_t j = 0; j < task->num_dependencies; j++) {
            Task* dependency = task->dependencies[j];
            if (!task_completed[i]) {
                ready_to_run = false;
                break;
            }
        }

        if (ready_to_run) {
            task->taskFunction();
            task_completed[i] = true;
            sleep_ms(task->delay_ms);
        }
    }
}

int main(int argc, char *argv[]) {
    init_peripherals();

    int loop_count = (argc > 1) ? atoi(argv[1]) : 0;
    bool infinite_loop = (loop_count == 0);

    // Création des tâches avec leurs fonctions, délais, priorités et poids
    static Task runTempTaskStruct = { runTempTask, 5000, 3, 1, NULL, 0 };
    static Task computeAvgTempTaskStruct = { computeAvgTempTask, 5000, 2, 2, NULL, 0 };
    static Task sendResultTaskStruct = { sendResultTask, 1000, 1, 3, NULL, 0 };
    
    static Task* dependencies[] = { &computeAvgTempTaskStruct };
    sendResultTaskStruct.dependencies = dependencies;
    sendResultTaskStruct.num_dependencies = 1;

    // Initialisation des dépendances
    Task* tasks[] = { &runTempTaskStruct, &computeAvgTempTaskStruct, &sendResultTaskStruct };
    uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);
    
    printf("Num Dep: %d\n", sendResultTaskStruct.num_dependencies);
    printf("Num Dep 0 tasks: %d\n", tasks[0]->num_dependencies);
    printf("Num Dep 1 tasks: %d\n", tasks[1]->num_dependencies);
    printf("Num Dep 2 tasks: %d\n", tasks[2]->num_dependencies);

    while (infinite_loop || loop_count>0) {
        execute_tasks(tasks, task_count);
        sleep_ms(1000);
        loop_count--;
    }
}
