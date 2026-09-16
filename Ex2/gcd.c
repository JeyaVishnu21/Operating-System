#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
int gcd(int a, int b) {
   while (b!=0) {
      int temp = b;
      b = a%b;
      a = temp;
   }
   return a;
}
int main () {
   int fd[2];
   pid_t pid;
   int n1,n2;
   if (pipe(fd)==-1) {
      perror("Pipe creation failed");
      exit(1);
   }
   pid=fork();
   if (pid==0) {
      close(fd[0]);
      printf("Child : Enter two numbers");
      scanf("%d %d",&n1,&n2);
      write(fd[1],&n1,sizeof(int));
      write(fd[1],&n2,sizeof(int));
      close(fd[1]);
   }
   else {
      close(fd[1]);
      read(fd[0],&n1,sizeof(int));
      read(fd[0],&n2,sizeof(int));
      close(fd[0]);
      wait(NULL);
      printf("Parent: Numbers recieved is %d and %d",n1,n2);
      printf("Parent: GCD is %d \n",gcd(n1,n2));
   }
   return 0;
}
