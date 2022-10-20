//n proccess -> n+1 pipes


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdbool.h> // Header-file for boolean data-type.

//prototypes
int Random(int max); 
int rollDice();
int recherche(int x, int (*pGameBoard)[2][56], int player, int horse, int nbPlayer);
void changeCase(int x, int (*pGameBoard)[2][56], int player, int horse, int caseToChange);
int isEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer);
//================================================================================================

int main(int argc, char* argv[]){
    //get value of n in argument
    int n = atoi(argv[1]);
    //array of pid
    int pidTab[n];


    //variable du jeu 
    int nextPlayer = 0;
    bool diceIsSix = false;
    int dice = 0;
    bool gameIsOver = false;
  
    //array of pid
    int caseMoved; 
    int i, j , k;

    int gameBoard[n][2][56];
    int lengthGb =0;
    for(i=0; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                
                    gameBoard[i][j][k] = 0;
                
                lengthGb++;
            }
        }
    }

    int (*pGameBoard)[2][56] = gameBoard;

    int oui=0;
    while (!gameIsOver){

        
        //create n+1 pipes
        int pipes[n+1][2];
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
                srand(time(NULL)+getpid());

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
                
                if(i == nextPlayer){
                    x = rollDice();
                }
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
        for(j=0; j<n+1; j++){
            if(j!=n){
                close(pipes[j][0]);
            }
            if(j!=0){
                close(pipes[j][1]);
            }
        }
        dice = 0;
        if(write(pipes[0][1], &dice, sizeof(int))< 0){
            printf("erreur ecriture pipe");
            return 5;
        }
        if(read(pipes[n][0], &dice, sizeof(int))< 0){
            printf("erreur lecture pipe");
            return 6;
        }
        printf("Main process read x : %d from pipe %d\n", dice, n);

       /* if(dice!=6){
                //if no horse 0 but horse 1 on the gameboard, horse 1 moves







            //if the lastplayer to play was n-1, the next player is 0
            if(nextPlayer == n-1){
                nextPlayer = 0;
            }
            //if the lastplayer to play wasnt n-1, the next player is the lastplayer +1
            else{
                nextPlayer++;
            }

        }*/

        //finishing close all pipes
        close(pipes[0][0]);
        close(pipes[n][1]);
        
        //wait for all children to finish
        for (i=0; i< n ; i ++){
            wait(NULL);
        }

       oui++;
        if(oui == 10  ){
            gameIsOver = true;
        }

    }


    //print gameboard
    for(i=0; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                printf("%d ", gameBoard[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    changeCase(1, pGameBoard, 0, 1, 29);
    //print gameboard
    printf("====================\n");
    for(i=0; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                printf("%d ", gameBoard[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("%d", isEmpty(pGameBoard, 0, 0, 29, n));
    
    return 0;
}

//================================================================================================

int Random(int Max) {
    return ( rand() % Max)+ 1;
}
int rollDice(){
    int d1=Random(6) ;
    return d1;
}
//if there is a horse in [player][horse] array, return his indice, else return -1
int recherche(int x, int (*pGameBoard)[2][56], int player, int horse, int nbPlayer){
    int i ,j ,k;
    int look = x;
    for ( i =0; i<nbPlayer; i++) { 
        for ( j =0; j <2; j++) {
            for ( k =0; k<56; k++){
                
                   if(*(*(*(pGameBoard +i) + j) +k) == look){
                          return k;
                   }
                
            }
         }
    } 
    return -1;
}
//change the case of the horse in [player][horse][caseToChange] array to x
void changeCase(int x, int (*pGameBoard)[2][56], int player, int horse, int caseToChange){
    int i ,j ,k;
    int look = x;
    i=player;
    j=horse;
    k=caseToChange;
     *(*(*(pGameBoard +i) + j) +k) = x;
      
}

//isEmpty function, check if the case is empty for the other players
int isEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer){
    int i ,j ,k;
    i = player;
    j = horse;
    k = caseToCheck;
    bool quatorze = false;
    for(int y=0; y<nbPlayer; y++){
        for(int x =0; x<2; x++){
                if(*(*(*(pGameBoard +y) + x) +k) != 0){
                    printf("\n case non vide :  player : %d, horse : %d, case : %d\n", y, x, k);
                    return 0;
                }
            
            printf("[%d][%d][%d] = %d ", y, x, k, *(*(*(pGameBoard +y) + x) +k));
            if (x==1){
                k+=14;
            }
            if(k>55){
                k-=56;

            }   
        }
        
    }
    return 1;
}