#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE_SIZE 10  // Maximum line size for input from TaskSpec.txt

// Structure representing a task (process)
typedef struct Task{
    char taskName[4];
    int arrivalTime;
    int burstTime;
    int remainingBurstTime;
    int startTime;
    int finishTime;
    int waitingTime;
} Task;

// Node structure for linked list implementation of a queue
typedef struct Node{
    Task task;
    struct Node* next;
} Node;

// Queue structure to hold the front and rear pointers
typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

// Create and initialize a new queue
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    q->front = q->rear = NULL;
    return q;
}

// Enqueue a task into the given queue
void enqueue(Queue* q, Task* task) {
    Node* temp = (Node*)malloc(sizeof(Node));
    if (temp == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    temp->task = *task;
    temp->next = NULL;
    if (q->rear == NULL) { // Queue is empty
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

// Dequeue a task from the given queue and return it
Task dequeue(Queue* q) {
    if (q->front == NULL) {
        printf("Queue is empty\n");
        exit (-1);
    }
    Node* temp = q->front;
    Task task = temp->task;
    q->front = q->front->next;
    if (q->front == NULL) { // Queue becomes empty
        q->rear = NULL;
    }
    free(temp);
    return task;
}

// Calculate and update waiting time for a task
void updateWaitingTime(Task* task) {
    task->waitingTime = task->finishTime - task->arrivalTime - task->burstTime;
}

// Print waiting time for each task and the average waiting time; results are appended to Output.txt
void printWaitingAndAverageTime(Queue* completedQueue, int totalTasks) {
    FILE *outFile = fopen("Output.txt", "a");
    if (outFile == NULL) {
        printf("Error: %s\n", strerror(errno));
        exit(1);
    }

    // Create an array to store tasks from the completed queue
    Task* tasks = (Task*)malloc(sizeof(Task) * totalTasks);
    if (tasks == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    int i = 0;
    Node* curr = completedQueue->front;
    while (curr != NULL && i < totalTasks) {
        tasks[i++] = curr->task;
        curr = curr->next;
    }

    // Sort tasks by arrival time (simple bubble sort)
    for (int i = 0; i < totalTasks - 1; i++) {
        for (int j = i + 1; j < totalTasks; j++) {
            if (tasks[i].arrivalTime > tasks[j].arrivalTime) {
                Task temp = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }
        }
    }

    // Calculate total waiting time and print waiting time for each task
    int totalWaiting = 0;
    for (i = 0; i < totalTasks; i++) {
        fprintf(outFile, "Waiting Time %s: %d\n", tasks[i].taskName, tasks[i].waitingTime);
        totalWaiting += tasks[i].waitingTime;
    }

    // Calculate and print the average waiting time (rounded to two decimal places)
    double average = (double)totalWaiting / totalTasks;
    fprintf(outFile, "Average Waiting Time: %.2f", average);

    free(tasks);
    fflush(outFile);
    fclose(outFile);
}

// Create a clone of the given queue
Queue* cloneQueue(Queue* original) {
    Queue* newQueue = createQueue();
    Node* current = original->front;
    while (current != NULL) {
        Task taskCopy = current->task;
        enqueue(newQueue, &taskCopy);
        current = current->next;
    }
    return newQueue;
}

// Remove and return the task with the shortest burst time from the ready queue (used for NSJF)
Task removeShortestBurstTask(Queue* readyQueue) {
    if (readyQueue->front == NULL) {
        printf("Ready queue is empty\n");
        exit(-1);
    }

    Node* curr = readyQueue->front;
    Node* prev = NULL;
    Node* minNode = readyQueue->front;
    Node* minPrev = NULL;

    while (curr != NULL) {
        if (curr->task.burstTime < minNode->task.burstTime) {
            minNode = curr;
            minPrev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    if (minPrev == NULL) {
        readyQueue->front = minNode->next;
    }
    else {
        minPrev->next = minNode->next;
    }

    if (minNode == readyQueue->rear) {
        readyQueue->rear = minPrev;
    }

    Task minTask = minNode->task;
    free(minNode);
    return minTask;
}

// Remove and return the task with the shortest remaining burst time from the ready queue (used for PSJF)
Task removeShortestRemainingBurstTask(Queue *readyQueue) {
    if (readyQueue->front == NULL) {
        printf("Ready queue is empty\n");
        exit(-1);
    }

    Node* curr = readyQueue->front;
    Node* prev = NULL;
    Node* minNode = readyQueue->front;
    Node* minPrev = NULL;

    while (curr != NULL) {
        if (curr->task.remainingBurstTime < minNode->task.remainingBurstTime) {
            minNode = curr;
            minPrev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    if (minPrev == NULL) {
        readyQueue->front = minNode->next;
    }
    else {
        minPrev->next = minNode->next;
    }

    if (minNode == readyQueue->rear) {
        readyQueue->rear = minPrev;
    }

    Task minTask = minNode->task;
    free(minNode);
    return minTask;
}

// Non-preemptive Shortest-Job-First scheduling simulation (NSJF)
void NSJF(Queue* masterQueue, int totalTasks) {
    int currentTime = 0;
    int completedTasks = 0;
    Queue* readyQueue = createQueue();
    Queue* completedQueue = createQueue();
    FILE *outFile = fopen("Output.txt", "w");
    if (outFile == NULL) {
        printf("Error: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(outFile, "NSJF:\n");

    // Main loop: continue until all tasks are completed
    while (completedTasks < totalTasks) {
        // Enqueue tasks from masterQueue to readyQueue which are arrived
        while (masterQueue->front != NULL && masterQueue->front->task.arrivalTime <= currentTime) {
            Task temp = dequeue(masterQueue);
            enqueue(readyQueue, &temp);
        }

        // If no tasks are ready, jump to the next arrival time
        if (readyQueue->front == NULL) {
            currentTime = masterQueue->front->task.arrivalTime;
            continue;
        }

        // Select the task with the shortest burst time (NSJF)
        Task selectedTask = removeShortestBurstTask(readyQueue);
        selectedTask.startTime = currentTime;
        currentTime += selectedTask.burstTime;
        selectedTask.finishTime = currentTime;
        updateWaitingTime(&selectedTask);
        enqueue(completedQueue, &selectedTask);

        // Write the execution segment to the output file
        fprintf(outFile, "%s\t%d\t%d\n", selectedTask.taskName, selectedTask.startTime, selectedTask.finishTime);
        completedTasks++;
    }

    fflush(outFile);
    printWaitingAndAverageTime(completedQueue, totalTasks);
    free(completedQueue);
    fclose(outFile);
}

// Preemptive Shortest-Job-First scheduling simulation (PSJF)
void PSJF(Queue* masterQueue, int totalTasks) {
    int currentTime = 0;
    int completedTasks = 0;
    int currentTaskActive = 0;
    Task currentTask;
    Queue* readyQueue = createQueue();
    Queue* completedQueue = createQueue();

    FILE *outFile = fopen("Output.txt", "a");
    if (outFile == NULL) {
        printf("Error: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(outFile, "\n\nPSJF:\n");

    // Main loop: continue until all tasks are completed
    while (completedTasks < totalTasks) {
        // Enqueue tasks from masterQueue to readyQueue that have arrived
        while (masterQueue->front != NULL && masterQueue->front->task.arrivalTime <= currentTime) {
            Task temp = dequeue(masterQueue);
            enqueue(readyQueue, &temp);
        }

        // If no task is active and no task is ready, jump to the next arrival time
        if (!currentTaskActive && readyQueue->front == NULL) {
            currentTime = masterQueue->front->task.arrivalTime;
            continue;
        }

        // If no task is currently active, select one from the ready queue
        if (!currentTaskActive) {
            currentTask = removeShortestRemainingBurstTask(readyQueue);
            currentTask.startTime = currentTime;
            currentTaskActive = 1;
        }

        // Check if a candidate task in ready queue should preempt the current task
        if (readyQueue->front != NULL) {
            Task candidate = removeShortestRemainingBurstTask(readyQueue);
            // If candidate's remaining burst is less than current task's remaining burst, preempt
            if (candidate.remainingBurstTime < currentTask.remainingBurstTime) {
                fprintf(outFile, "%s\t%d\t%d\n", currentTask.taskName, currentTask.startTime, currentTime);
                enqueue(readyQueue, &currentTask);
                currentTask = candidate;
                currentTask.startTime = currentTime;
            }
            else {
                // Otherwise, re-enqueue the candidate task
                enqueue(readyQueue, &candidate);
            }
        }

        currentTime++;  // Advance simulation time by 1 millisecond
        currentTask.remainingBurstTime --;  // Decrease remaining burst time for current task

        // If current task completes execution
        if (currentTask.remainingBurstTime == 0) {
            currentTask.finishTime = currentTime;
            updateWaitingTime(&currentTask);
            enqueue(completedQueue, &currentTask);
            fprintf(outFile, "%s\t%d\t%d\n", currentTask.taskName, currentTask.startTime, currentTime);
            completedTasks++;
            currentTaskActive = 0;  // Mark no active task
        }
    }

    fflush(outFile);
    printWaitingAndAverageTime(completedQueue, totalTasks);
    free(completedQueue);
    fclose(outFile);
}

int main(void) {

    Queue* masterQueue = createQueue();

    int totalTasks = 0;
    char inputBuffer[MAX_LINE_SIZE];  // Buffer to hold the file input

    // Open TaskSpec.txt for reading; the input file should be in the same directory
    FILE* inputFile = fopen("TaskSpec.txt", "r");
    if (inputFile == NULL) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    // Read each line from TaskSpec.txt and enqueue tasks into the master queue
    while (fgets(inputBuffer, sizeof(inputBuffer), inputFile) != NULL) {
        Task t ;
        if (sscanf(inputBuffer, "%3[^,],%d,%d", t.taskName, &t.arrivalTime, &t.burstTime) == 3) {
            t.remainingBurstTime = t.burstTime;
            t.startTime = -1;
            t.finishTime = -1;
        }
        else {
            printf("Error parsing line: %s\n", inputBuffer);
            exit(1);
        }
        enqueue(masterQueue, &t);
        totalTasks++;
    }
    fclose(inputFile);

    // Clone the master queue for NSJF scheduling; NSJF uses one clone, PSJF uses the original
    Queue* masterQueueClone = cloneQueue(masterQueue);
    NSJF(masterQueueClone, totalTasks);
    PSJF(masterQueue, totalTasks);

    free(masterQueueClone);
    free(masterQueue);

    return 0;
}
