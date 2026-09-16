#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipe1[2], pipe2[2];
    pid_t p2, p3;
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("Pipe");
        exit(1);
    }

    p2 = fork();
    if (p2 == -1) { perror("Fork P2"); exit(1); }

    if (p2 == 0) {
        close(pipe2[0]);
        p3 = fork();
        if (p3 == -1) { perror("Fork P3"); exit(1); }

        if (p3 == 0) {
            close(pipe1[0]);
            close(pipe2[1]);

            int n, arr[100];
            printf("Enter number of elements: ");
            if(scanf("%d", &n) != 1) exit(1);

            printf("Enter array elements:\n");
            int i;
            for (i = 0; i < n; i++) {
                if(scanf("%d", &arr[i]) != 1) exit(1);
            }

            write(pipe1[1], &n, sizeof(int));
            write(pipe1[1], arr, n * sizeof(int));

            close(pipe1[1]);
            exit(0);
        }
        else  {
            close(pipe1[1]);
            int n, arr[100], odd[100];
            int count = 0;

            read(pipe1[0], &n, sizeof(int));
            read(pipe1[0], arr, n * sizeof(int));

            close(pipe1[0]);
            wait(NULL);
            int i;
            for (i = 0; i < n; i++)
            {
                if (arr[i] % 2 != 0)
                    odd[count++] = arr[i];
            }

            write(pipe2[1], &count, sizeof(int));
            write(pipe2[1], odd, count * sizeof(int));

            close(pipe2[1]);
            exit(0);
        }
    }
    else  {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        int i,n, arr[100];
        read(pipe2[0], &n, sizeof(int));
        read(pipe2[0], arr, n * sizeof(int));

        wait(NULL);
        close(pipe2[0]);
        printf("Array after removing the even elements");
        for(i=0;i<n;i++) {
           printf("%d\t",arr[i]); }

        if (n < 2)
        {
            printf("Second minimum not possible.\n");
        }
        else
        {
            int min1 = arr[0], min2 = 2147483647;
            int i;
            for (i = 1; i < n; i++)
            {
                if (arr[i] < min1)
                {
                    min2 = min1;
                    min1 = arr[i];
                }
                else if (arr[i] < min2 && arr[i] != min1)
                {
                    min2 = arr[i];
                }
                else if (min1 == min2 && arr[i] > min1)
                {
                    min2 = arr[i];
                }
            }

            if (min2 == 2147483647 || min1 == min2)
                printf("Second minimum not found.\n");
            else
                printf("Second minimum = %d\n", min2);
        }
    }
    return 0;
}
