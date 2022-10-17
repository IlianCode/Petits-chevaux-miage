#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdbool.h> // Header-file for boolean data-type.
#include <time.h> /* Needed just for srand seed */

// Function prototype.
void Randomize(); 
int Random(int max); 
int rollDice();
int recherche(int x, int *tabl, int taille);


int main(int argc, char* argv[]){
    //main process of a little horses game with n players
    //variable of the game
    int nextPlayer = 1;
    bool diceIsSix = false;
    int dice = 0;
    bool gameIsOver = false;
    //get value of n in argument
    int n = atoi(argv[1]);
    //array of pid
    int pidTab[n];
    


    //create an array 3D n,2,56 cases each for the gameboard and fill it with 0
    int gameBoard[n][2][56];
    int i;
    int j;
    int k;
    for(i=0; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                gameBoard[i][j][k] = 0;
            }
        }
    }
    //create an array of 6 cases for the final stairs and fill it with 0
    int stair[6];
    for(i=0; i<6; i++){
        stair[i]=0;
    }


    int oui =0;
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
    if(write(pipes[0][1], &dice, sizeof(int))< 0){
        printf("erreur ecriture pipe");
        return 5;
    }
    if(read(pipes[n][0], &dice, sizeof(int))< 0){
        printf("erreur lecture pipe");
        return 6;
    }

    printf("Main process read x : %d from pipe %d\n", dice, n);
    printf("player %d rolled a %d\n", nextPlayer, dice);
    if(dice != 6){
        
        //if no horse 0 but horse 1 on the gameboard, horse 1 moves
        if(recherche(nextPlayer, gameBoard[nextPlayer][0], 56) == -1){
            if(recherche(nextPlayer, gameBoard[nextPlayer][1], 56)){

                int indiceCheval = recherche(1,gameBoard[nextPlayer][0],56);
                gameBoard[nextPlayer][1][indiceCheval] = 0;
                gameBoard[nextPlayer][1][indiceCheval + dice] = 1;
            }
        }
        //if no horse 1 but horse 0 on the gameboard, horse 0 move
        else if(recherche(nextPlayer, gameBoard[nextPlayer][0], 56) != -1){
            if(recherche(nextPlayer, gameBoard[nextPlayer][1], 56) == -1){
                int indiceCheval = recherche(1,gameBoard[nextPlayer][0],56);
                gameBoard[nextPlayer][0][indiceCheval] = 0;
                gameBoard[nextPlayer][0][indiceCheval + dice] = 1;
            }
        }else if (recherche(nextPlayer, gameBoard[nextPlayer][0], 56) != -1 && recherche(nextPlayer, gameBoard[nextPlayer][1], 56) != -1){
            //if two horse on the gameboard, ask which one to move
            printf("Which horse do you want to move ? (0 or 1)\n");
            int horse;
            scanf("%d", &horse);
            if(horse == 0){
                int indiceCheval = recherche(1,gameBoard[nextPlayer][0],56);
                gameBoard[nextPlayer][0][indiceCheval] = 0;
                gameBoard[nextPlayer][0][indiceCheval + dice] = 1;
            }else if(horse == 1){
                int indiceCheval = recherche(1,gameBoard[nextPlayer][1],56);
                gameBoard[nextPlayer][1][indiceCheval] = 0;
                gameBoard[nextPlayer][1][indiceCheval + dice] = 1;
            }
        }

        //if the player doesnt have a horse on the gameboard
        //if the lastplayer to play was n-1, the nex player is 0
        if(nextPlayer == n-1){
            nextPlayer = 0;
        }
        //if the lastplayer to play wasnt n-1, the next player is the lastplayer +1
        else{
            nextPlayer = (nextPlayer + 1) % n;
        }


    }else{
        diceIsSix = true;
        if(recherche(1,gameBoard[nextPlayer][0],56) == -1){
            gameBoard[nextPlayer][0][0] = 1;
        }else{
            if(recherche(1,gameBoard[nextPlayer][1],56) == -1){
                printf("Voulez vous sortir un cheval de l'écurie ou avancer votre premier cheval ? (1 ou 2)\n");
                int choice;
                scanf("%d", &choice);
                if(choice == 1){
                    gameBoard[nextPlayer][1][0] = 1;
                    printf("Vous avez sorti votre deuxieme cheval de l'écurie\n");
                }else if(choice == 2){
                    int indicePremierCheval = recherche(1,gameBoard[nextPlayer][0],56);
                    gameBoard[nextPlayer][0][indicePremierCheval] = 0;
                    gameBoard[nextPlayer][0][indicePremierCheval + dice] = 1;
                    printf("Vous avez avancé le premier cheval à la case %d\n", indicePremierCheval + dice);

                }
            }else{
                printf("Voulez vous avancer le premier cheval ou le deuxième ? (1 ou 2)\n");
                int choice;
                scanf("%d", &choice);
                if(choice == 1){
                    int indicePremierCheval = recherche(1,gameBoard[nextPlayer][0],56);
                    gameBoard[nextPlayer][0][indicePremierCheval] = 0;
                    gameBoard[nextPlayer][0][indicePremierCheval + dice] = 1;
                    printf("Vous avez avancé le premier cheval à la case %d\n", indicePremierCheval + dice);
                }else if(choice == 2){
                    int indiceDeuxiemeCheval = recherche(1,gameBoard[nextPlayer][1],56);
                    gameBoard[nextPlayer][1][indiceDeuxiemeCheval] = 0;
                    gameBoard[nextPlayer][1][indiceDeuxiemeCheval + dice] = 1;
                    printf("Vous avez avancé le deuxième cheval à la case %d\n", indiceDeuxiemeCheval + dice);
                }

            }
        }
    }
    printf("next player is %d\n", nextPlayer);

    //finishing close all pipes
    close(pipes[0][0]);
    close(pipes[n][1]);
    
    //wait for all children to finish
    for (i=0; i< n ; i ++){
        wait(NULL);
    }
    oui++;
    if(oui == 4 ){
        gameIsOver = true;
    }
    }
    return 0;
    
}


//initialize the random number generator for the dice roll
void Randomize() {
    srand( (unsigned)time( NULL ) ) ;
}
int Random(int Max) {
    return ( rand() % Max)+ 1;
}
int rollDice(){
    Randomize() ;
    int d1=Random(6) ;
    return d1;
}
//end of the dice roll functions

int recherche(int x, int *tabl, int taille){
    int res;
    for (int i =0; i< taille; i++){
        if (tabl[i]== x){
            res= i;
            return res;
        }
    }
    return -1;
}


