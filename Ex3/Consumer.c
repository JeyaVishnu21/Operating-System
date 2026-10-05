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

    shmid = shmget(4777, sizeof(int) * (SIZE + 2), 0666);

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

    printf("Data Read:\n");

    for (i = 0; i < SIZE; i++)
    {
        // BUSY WAIT: Loop while buffer is empty
        while (buf[10] == buf[11])
        {
            // Do nothing, just spin
        }

        num = buf[buf[11]];
        printf("%d\n", num);

        buf[11] = (buf[11] + 1) % SIZE;
    }

    shmdt(buf);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
