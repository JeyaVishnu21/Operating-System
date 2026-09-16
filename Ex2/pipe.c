#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
int main() {
   int fd[2];
   pid_t pid;
   char writemsg[] = "Hello";
   char readmsg[100];
   if(pipe(fd)==-1) {
      perror("Pipe creation failed");
      return 1;
   }
   pid=fork();
   if(pipe<0){
      perror("Fork creation failed");
      return 1;
   }
   if(pid==0) {
      close(fd[1]);
      int bytes_read = read(fd[0],readmsg,sizeof(readmsg)-1);
      if(bytes_read>0) {
         readmsg[bytes_read]='\0';
         printf("Child recieved : %s\n",readmsg);
      }
      close(fd[0]);
   }
   else {
      close(fd[0]);
      printf("Parent sending:%s\n",writemsg);
      write(fd[1],writemsg,strlen(writemsg));
      close(fd[1]);
      wait(NULL);
   }
   return 0;
}
