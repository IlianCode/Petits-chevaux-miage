//n proccess -> n+1 pipes


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]){
    //get value of n in argument
    int n = atoi(argv[1]);
    //array of pid
    int pidTab[n];
    //create n+1 pipes
    int pipes[n+1][2];
    int i;
    for(i = 0; i < n+1; i++){
        if(pipe(pipes[i]) ==-1){
            printf("erreur pipe");
            return 1;
        } 
    }


    //create n+1 children
    for (i=0; i< n ; i ++){
        pidTab[i] = fork();
        if(pidTab[i]==-1){
            printf("erreur création processus");
            return 2;
        }
        if(pidTab[i]==0){
            //child process
            //close all pipes except the one he is using
            int j;
            for(j=0; j<n+1; j++){
                if(j!=i){
                    close(pipes[j][0]);
                }
                if(j!=i+1){
                    close(pipes[j][1]);
                }
            }
            //read from pipe i
            int x;
            if(read(pipes[i][0], &x, sizeof(int))< 0){
                printf("erreur lecture pipe");
                return 3;
            }
            printf("Child %d read x : %d from pipe %d\n", i, x, i);
            //write to pipe i+1
            
            if(write(pipes[i+1][1], &x, sizeof(int))< 0){
                printf("erreur ecriture pipe");
                return 4;
            }
            //close all pipes
            close(pipes[i][0]);
            close(pipes[i+1][1]);
            
            return 0;
        }
    }


    //main process

    //close all pipes except the first one
    int j;
    for(j=0; j<n+1; j++){
        if(j!=n){
            close(pipes[j][0]);
        }
        if(j!=0){
            close(pipes[j][1]);
        }
    }
    int y = 0;
    if(write(pipes[0][1], &y, sizeof(int))< 0){
        printf("erreur ecriture pipe");
        return 5;
    }
    if(read(pipes[n][0], &y, sizeof(int))< 0){
        printf("erreur lecture pipe");
        return 6;
    }

    printf("Main process read x : %d from pipe %d\n", y, n);
    //finishing close all pipes
    close(pipes[0][0]);
    close(pipes[n][1]);
    
    //wait for all children to finish
    for (i=0; i< n ; i ++){
        wait(NULL);
    }



    return 0;
}