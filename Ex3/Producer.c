#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
    int shmid, *buf;
    int i, num;
    int SIZE = 10;

    shmid = shmget(4777, sizeof(int) * (SIZE + 2), IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("shmget");
        return 0;
    }

    buf = (int *)shmat(shmid, NULL, 0);

    if (buf == (void *)-1)
    {
        perror("shmat");
        return 0;
    }

    buf[10] = 0; // in pointer
    buf[11] = 0; // out pointer

    for (i = 0; i < SIZE; i++)
    {
        printf("Enter data: ");
        scanf("%d", &num);

        // BUSY WAIT: Loop while buffer is full
        while (((buf[10] + 1) % SIZE) == buf[11])
        {
            // Do nothing, just spin
        }

        buf[buf[10]] = num;
        buf[10] = (buf[10] + 1) % SIZE;
    }

    shmdt(buf);

    return 0;
}
