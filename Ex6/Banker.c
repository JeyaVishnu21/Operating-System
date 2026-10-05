#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 20
#define MAX_RESOURCES 20

// Global Variables
int n, m;
int allocation[MAX_PROCESSES][MAX_RESOURCES];
int maximum[MAX_PROCESSES][MAX_RESOURCES];
int need[MAX_PROCESSES][MAX_RESOURCES];
int available[MAX_RESOURCES];

/* Calculate Need Matrix */
void calculateNeed() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

/* Display System Matrices Side-by-Side */
void printSystemState() {
    printf("\n%-8s | %-15s | %-15s | %-15s\n", "Process", "Allocation", "Maximum", "Remaining Need");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("P%-7d | ", i + 1);
        
        // Print Allocation
        for (int j = 0; j < m; j++) printf("%d ", allocation[i][j]);
        printf("\t\t | ");
        
        // Print Maximum
        for (int j = 0; j < m; j++) printf("%d ", maximum[i][j]);
        printf("\t\t | ");
        
        // Print Need
        for (int j = 0; j < m; j++) printf("%d ", need[i][j]);
        printf("\n");
    }
    
    printf("\nCurrent Available Vector: [ ");
    for (int j = 0; j < m; j++) printf("%d ", available[j]);
    printf("]\n");
}

/* Safety Core Algorithm Execution */
bool isSafeState(bool display_steps) {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES] = {false};
    int safeSequence[MAX_PROCESSES];
    int count = 0;

    for (int j = 0; j < m; j++) {
        work[j] = available[j];
    }

    while (count < n) {
        bool found = false;

        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool canFinish = true;
                for (int j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        canFinish = false;
                        break;
                    }
                }

                if (canFinish) {
                    for (int j = 0; j < m; j++) {
                        work[j] += allocation[i][j];
                    }
                    
                    if (display_steps) {
                        printf(" -> [Step %d] P%d running. Released resources. New Work: ", count + 1, i + 1);
                        for (int j = 0; j < m; j++) printf("%d ", work[j]);
                        printf("\n");
                    }

                    safeSequence[count++] = i;
                    finish[i] = true;
                    found = true;
                    break; // Restart scan loop with updated resources
                }
            }
        }

        if (!found) {
            if (display_steps) printf("\n[!] Execution blocked: System is UNSAFE (Deadlock Hazard).\n");
            return false;
        }
    }

    if (display_steps) {
        printf("\n>>> SUCCESS: System is in a SAFE state.");
        printf("\n>>> Execution Order: < ");
        for (int i = 0; i < n; i++) {
            printf("P%d ", safeSequence[i] + 1);
            if (i < n - 1) printf("-> ");
        }
        printf(">\n");
    }

    return true;
}

/* Resource Request Simulator */
void resourceRequest(int process) {
    int request[MAX_RESOURCES];

    printf("\nEnter resource units requested by P%d:\n", process + 1);
    for (int j = 0; j < m; j++) {
        printf("  Resource R%d: ", j + 1);
        if (scanf("%d", &request[j]) != 1) return;
    }

    // Constraint Validation
    for (int j = 0; j < m; j++) {
        if (request[j] > need[process][j]) {
            printf("\n[ERROR] Request rejected. Exceeds declared maximum Need limits.\n");
            return;
        }
        if (request[j] > available[j]) {
            printf("\n[WAIT] Request deferred. Exceeds current available pool.\n");
            return;
        }
    }

    // Speculative Allocation
    for (int j = 0; j < m; j++) {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    printf("\nSimulating tracking safety sequence given the new configuration...\n");

    if (isSafeState(false)) {
        printf("\n[APPROVED] Request successfully granted! The system remains secure.\n");
        isSafeState(true); // Display the updated tracking order
    } else {
        // Rollback state change
        for (int j = 0; j < m; j++) {
            available[j] += request[j];
            allocation[process][j] -= request[j];
            need[process][j] += request[j];
        }
        printf("\n[DENIED] Request rolled back. Granting this allocation compromises safety.\n");
    }
}

int main() {
    char workGiven;
    int choice;

    printf("===================================================\n");
    printf("           BANKER'S ALGORITHM ENGINE               \n");
    printf("===================================================\n");

    printf("Enter number of processes: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROCESSES) return 1;

    printf("Enter number of resource types: ");
    if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_RESOURCES) return 1;

    // Data Allocation Matrix
    printf("\n--- INPUT ALLOCATION MATRIX ---\n");
    for (int i = 0; i < n; i++) {
        printf("Process P%d: ", i + 1);
        for (int j = 0; j < m; j++) {
            if (scanf("%d", &allocation[i][j]) != 1) return 1;
        }
    }

    // Maximum Demand Matrix
    printf("\n--- INPUT MAXIMUM DEMAND MATRIX ---\n");
    for (int i = 0; i < n; i++) {
        printf("Process P%d: ", i + 1);
        for (int j = 0; j < m; j++) {
            if (scanf("%d", &maximum[i][j]) != 1) return 1;
            if (maximum[i][j] < allocation[i][j]) {
                printf("FATAL: Maximum demand cannot fall below current Allocation!\n");
                return 1;
            }
        }
    }

    calculateNeed();

    // Available Vector Logic
    printf("\nIs the initial Available Vector known? (y/n): ");
    if (scanf(" %c", &workGiven) != 1) return 1;

    if (workGiven == 'y' || workGiven == 'Y') {
        printf("Enter Available elements: ");
        for (int j = 0; j < m; j++) {
            if (scanf("%d", &available[j]) != 1) return 1;
        }
    } else {
        int total[MAX_RESOURCES] = {0};
        int allocatedSum[MAX_RESOURCES] = {0};

        printf("Enter Total instance limits for resources: ");
        for (int j = 0; j < m; j++) {
            if (scanf("%d", &total[j]) != 1) return 1;
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                allocatedSum[j] += allocation[i][j];
            }
        }

        for (int j = 0; j < m; j++) {
            available[j] = total[j] - allocatedSum[j];
            if (available[j] < 0) {
                printf("FATAL: Allocation sum exceeds total capacity limits!\n");
                return 1;
            }
        }
    }

    printSystemState();

    printf("\n===== RUNNING INITIAL SAFETY ASSESSMENT =====\n");
    if (!isSafeState(true)) {
        printf("\nSystem initialization aborted: Starting configurations are unsafe.\n");
        return 0;
    }

    // Operations Loop
    while (1) {
        printf("\n--- MENU OPERATIONS ---\n");
        printf("1. Allocate Resource Request\n");
        printf("2. View Full State Profile\n");
        printf("3. Recalculate System Safety\n");
        printf("4. Shutdown Simulator\n");
        printf("\nAction Selection: ");
        
        if (scanf("%d", &choice) != 1) return 1;

        switch (choice) {
            case 1: {
                int pid;
                printf("Target Process ID (1 to %d): ", n);
                if (scanf("%d", &pid) == 1) {
                    if (pid < 1 || pid > n) printf("Invalid Process boundary.\n");
                    else resourceRequest(pid - 1);
                }
                break;
            }
            case 2:
                printSystemState();
                break;
            case 3:
                printf("\n===== RERUNNING SAFETY SEQUENCE =====\n");
                isSafeState(true);
                break;
            case 4:
                printf("\nTerminating simulator engine. Goodbye!\n");
                return 0;
            default:
                printf("Option unknown. Re-enter option.\n");
        }
    }

    return 0;
}
