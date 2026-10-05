#include "header.h"

int main() {
    int shmid, semid;
    struct shared_buffer *shm;

    /* Access existing shared memory */
    shmid = shmget(SHM_KEY, sizeof(struct shared_buffer), 0666);
    if (shmid < 0) {
        perror("shmget failed. Run producer first.");
        exit(EXIT_FAILURE);
    }

    /* Attach shared memory */
    shm = (struct shared_buffer *)shmat(shmid, NULL, 0);
    if ((void *)shm == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    /* Access existing semaphores */
    semid = semget(SEM_KEY, 3, 0666);
    if (semid < 0) {
        perror("semget failed. Run producer first.");
        exit(EXIT_FAILURE);
    }

    printf("=== Consumer Started ===\n");
    printf("Consuming integers from circular buffer...\n");

    while (1) {
        /* Wait for filled slot and lock mutex */
        wait_sem(semid, FULL);
        wait_sem(semid, MUTEX);

        /* Read integer from circular buffer */
        int num = shm->buffer[shm->out];
        shm->out = (shm->out + 1) % BUFFER_SIZE;

        /* Unlock mutex and signal that a slot is now empty */
        signal_sem(semid, MUTEX);
        signal_sem(semid, EMPTY);

        printf("[Consumer] Consumed: %d\n", num);

        if (num == -1) {
            printf("[Consumer] Termination signal (-1) received. Exiting.\n");
            break;
        }
    }

    /* Detach shared memory */
    shmdt(shm);

    /* Cleanup: remove shared memory and semaphores */
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
