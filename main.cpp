#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

using namespace std;




char *args[10000];
char input[10000];

bool wait_for_input(){
    gets(input);

    return strlen(input) > 0;
}

void parse(){

    char space[2] = " ";
    char *tokens = strtok(input,space);

    int idx = 0;
    while(tokens != NULL){

        args[idx++] = tokens;

        tokens = strtok(NULL,space);
    }


    args[idx] = NULL;


}
bool should_i_block(){
   if(args[1] != NULL){
      if(strcmp(args[1] , "&") == 0)
        return 0;
   }
   return 1;
}

int cd(char *path){ /// returns -1 in case of failure
   return chdir(path);
}

FILE *fp;

void sighandler(int x){

   fputs("child died\n" , fp);

}

int main()
{

    fp = fopen("history.log","a+");

    signal(SIGCHLD , sighandler);

    while(1){

          printf("ahmed >> ");
          if(wait_for_input() == 0)
              continue;


          parse();

          if(strcmp(args[0] , "cd") == 0){
               if(cd(args[1]) < 0){
                perror(args[1]);
               }

               continue;
          }

          if(strcmp(args[0],"exit") == 0){
            exit(EXIT_SUCCESS);
          }
          pid_t child = fork();

          if(child == -1){
              perror("fork");
              exit(EXIT_FAILURE);
          }

          if(child == 0){ /// this is the child image
              if(execvp(args[0] , args)<0){
                    perror(args[0]);
                    exit(1);
              }
              exit(EXIT_SUCCESS);
          }
          else{
            /// waitpid doesnot necessarily wait for the child to terminate so , it is more optimal to use for our project
            /// stat_loc is a pointer to an int indicating terminating status of process ,
            /// several macros :
            /// WIFEXITED -> normal termination
            /// WIFSIGNALED -> abnormal

            /// waitpind(pid_child , &stat_loc , option)
            /// options :
            /// WNOHANG makes it non blocking
            /// WUNTRACED makes it blocking

             int status;
             if(should_i_block() == 1)
             {
                   status = waitpid(child,&status,WUNTRACED);

             }
             else{

                   status = waitpid(child,&status,WNOHANG);
             }
          }

    }

       fclose(fp);

    return 0;

}

/// SIGCHLD -> signal the mechanism processes communicate with each other or between processes and kernel
