#include "header.h"

int main() {
    int shmid, semid;
    struct shared_buffer *shm;

    /* Create shared memory */
    shmid = shmget(SHM_KEY, sizeof(struct shared_buffer), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    /* Attach shared memory */
    shm = (struct shared_buffer *)shmat(shmid, NULL, 0);
    if ((void *)shm == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    /* Initialize circular buffer indices */
    shm->in = 0;
    shm->out = 0;

    /* Create 3 semaphores: mutex, full, empty */
    semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    /* Initialize semaphore values */
    union semun u;
    u.val = 1;               /* mutex = 1 (binary semaphore) */
    semctl(semid, MUTEX, SETVAL, u);
    u.val = 0;               /* full = 0 (no items yet) */
    semctl(semid, FULL, SETVAL, u);
    u.val = BUFFER_SIZE;     /* empty = BUFFER_SIZE (all slots free) */
    semctl(semid, EMPTY, SETVAL, u);

    int num;
    printf("=== Producer Started ===\n");
    printf("Enter integers (-1 to terminate):\n");

    while (1) {
        printf("Enter number: ");
        if (scanf("%d", &num) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        /* Wait for empty slot and lock mutex */
        wait_sem(semid, EMPTY);
        wait_sem(semid, MUTEX);

        /* Write integer to circular buffer */
        shm->buffer[shm->in] = num;
        shm->in = (shm->in + 1) % BUFFER_SIZE;

        /* Unlock mutex and signal that a new item is available */
        signal_sem(semid, MUTEX);
        signal_sem(semid, FULL);

        printf("[Producer] Produced: %d\n", num);

        if (num == -1) {
            printf("[Producer] Termination signal sent. Exiting.\n");
            break;
        }
    }

    /* Detach shared memory */
    shmdt(shm);

    return 0;
}

