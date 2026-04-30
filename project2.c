#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Maximum number of processes we can handle
#define MAX_PROCESSES 20

// Size of the shared buffer
#define BUFFER_SIZE 5

// Structure to store process info
typedef struct {
    int pid;
    int burstTime;
} Process;

// Array to store processes from file
Process processes[MAX_PROCESSES];
int processCount = 0;

// Shared buffer for Producer-Consumer
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

// Synchronization tools
pthread_mutex_t mutex;   // protects buffer
sem_t empty;             // counts empty slots
sem_t full;              // counts filled slots

// Function for each process thread
void *processThread(void *arg) {
    Process *p = (Process *)arg;

    // Print when process starts
    printf("[Process %d] Started with CPU burst time %d seconds\n", p->pid, p->burstTime);

    // Simulate CPU execution
    sleep(p->burstTime);

    // Print when process finishes
    printf("[Process %d] Finished execution\n", p->pid);

    return NULL;
}

// Producer thread: adds items to buffer
void *producer(void *arg) {
    for (int i = 0; i < processCount; i++) {

        printf("[Producer] Waiting for empty slot...\n");
        sem_wait(&empty); // wait if buffer is full

        pthread_mutex_lock(&mutex); // lock buffer

        // Add process ID to buffer
        buffer[in] = processes[i].pid;
        printf("[Producer] Added Process %d to buffer at position %d\n", processes[i].pid, in);

        in = (in + 1) % BUFFER_SIZE; // move index circularly

        pthread_mutex_unlock(&mutex); // unlock buffer

        sem_post(&full); // signal that buffer has new item

        sleep(1);
    }

    return NULL;
}

// Consumer thread: removes items from buffer
void *consumer(void *arg) {
    for (int i = 0; i < processCount; i++) {

        printf("[Consumer] Waiting for item...\n");
        sem_wait(&full); // wait if buffer is empty

        pthread_mutex_lock(&mutex); // lock buffer

        // Remove item from buffer
        int item = buffer[out];
        printf("[Consumer] Removed Process %d from buffer at position %d\n", item, out);

        out = (out + 1) % BUFFER_SIZE; // move index circularly

        pthread_mutex_unlock(&mutex); // unlock buffer

        sem_post(&empty); // signal that buffer has space

        sleep(1);
    }

    return NULL;
}

int main() {
    // Open input file
    FILE *file = fopen("processes.txt", "r");

    if (file == NULL) {
        printf("Error: Could not open processes.txt\n");
        return 1;
    }

    // Read processes from file
    while (fscanf(file, "%d %d", &processes[processCount].pid,
                  &processes[processCount].burstTime) == 2) {
        processCount++;
    }

    fclose(file);

    printf("Loaded %d processes from processes.txt\n\n", processCount);

    pthread_t threads[MAX_PROCESSES];

    printf("=== Process Thread Simulation ===\n");

    // Create a thread for each process
    for (int i = 0; i < processCount; i++) {
        pthread_create(&threads[i], NULL, processThread, &processes[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < processCount; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n=== Producer-Consumer Synchronization ===\n");

    pthread_t producerThread, consumerThread;

    // Initialize mutex and semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    // Start producer first to avoid empty buffer issue
    pthread_create(&producerThread, NULL, producer, NULL);
    sleep(1);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    // Wait for both threads to finish
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    // Clean up synchronization tools
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("\nSimulation complete.\n");

    return 0;
}
