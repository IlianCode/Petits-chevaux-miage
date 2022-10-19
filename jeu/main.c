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
int recherche(int x, int *tabl, int player , int horse, int taille);
void changeCase(int player, int horse, int caseBoard, int *array, int value);
int isCaseEmpty(int player, int horse, int caseBoard, int *array, int nbPlayer);


int main(int argc, char* argv[]){
    //main process of a little horses game with n players
    //variable of the game
    int nextPlayer = 0;
    bool diceIsSix = false;
    int dice = 0;
    bool gameIsOver = false;
    //get value of n in argument
    int n = atoi(argv[1]);
    //array of pid
    int pidTab[n];
    int caseMoved; 
    int i, j , k;
 

    //create an array 3D n,2,56 cases each for the gameboard and fill it with 0
    int *gameBoard = malloc(n*2*56*sizeof(int));
    int lengthOfGb =0;
    for(i=1; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                lengthOfGb++;
            }
        }
    }
    for(i=0; i<lengthOfGb; i++){
        gameBoard[i] =0;
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
                printf("new dice rolled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
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
    dice =0;

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
        if(recherche(1, gameBoard,nextPlayer,0, lengthOfGb) == -1){
            if(recherche(1, gameBoard, nextPlayer,1, lengthOfGb)!= -1){

                int indiceCheval = recherche(1,gameBoard,nextPlayer,1,lengthOfGb);
                //before moving the horse, check if there is a horse on the case where the horse is going to move
                int checkIfEmpty =isCaseEmpty(nextPlayer,1,indiceCheval+dice,gameBoard,n);
                if(checkIfEmpty==-1){
                    changeCase(nextPlayer,1,indiceCheval,gameBoard, 0);
                    changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);    
                }else{
                    //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                    changeCaseToZero(checkIfEmpty,gameBoard);
                    changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);
                }
                
                caseMoved = ((indiceCheval + dice) % nextPlayer*2*56);
                printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

               // gameBoard[nextPlayer][1][indiceCheval] = 0;
               // gameBoard[nextPlayer][1][indiceCheval + dice] = 1;
            }
        }
        //if no horse 1 but horse 0 on the gameboard, horse 0 move
        else if(recherche(1, gameBoard,nextPlayer , 0, lengthOfGb) != -1){
            if(recherche(1, gameBoard, nextPlayer, 1, lengthOfGb) == -1){
                int indiceCheval = recherche(1,gameBoard,nextPlayer, 0,lengthOfGb);
                //before moving the horse, check if there is a horse on the case where the horse is going to move
                int checkIfEmpty =isCaseEmpty(nextPlayer,0,indiceCheval+dice,gameBoard,n);
                if(checkIfEmpty==-1){
                    changeCase(nextPlayer,0,indiceCheval,gameBoard, 0);
                    changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);
                }else{
                    //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                    changeCaseToZero(checkIfEmpty,gameBoard);
                    changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);
                }
               // changeCase(nextPlayer,0,indiceCheval,gameBoard, 0);
                //changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);

                caseMoved = ((indiceCheval + dice) % nextPlayer*2*56);
                printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

                //gameBoard[nextPlayer][0][indiceCheval] = 0;
                //gameBoard[nextPlayer][0][indiceCheval + dice] = 1;
            }
        }else if (recherche(1, gameBoard,nextPlayer,0, lengthOfGb) != -1 && recherche(1, gameBoard,nextPlayer,1, lengthOfGb) != -1){
            //if two horse on the gameboard, ask which one to move
            printf("Quel cheval voulez vous bouger ? (0 or 1)\n");
            int horse;
            scanf("%d", &horse);
            if(horse == 0){
                int indiceCheval = recherche(1,gameBoard,nextPlayer,0,lengthOfGb);
                //before moving the horse, check if there is a horse on the case where the horse is going to move
                int checkIfEmpty =isCaseEmpty(nextPlayer,0,indiceCheval+dice,gameBoard,n);
                if(checkIfEmpty==-1){
                    changeCase(nextPlayer,0,indiceCheval,gameBoard, 0);
                    changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);
                }else{
                    //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                    changeCaseToZero(checkIfEmpty,gameBoard);
                    changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);
                }
                //changeCase(nextPlayer,0,indiceCheval,gameBoard, 0);
                //changeCase(nextPlayer,0,indiceCheval+dice ,gameBoard, 1);
                caseMoved = ((indiceCheval + dice) % nextPlayer*2*56);
                printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);


               // gameBoard[nextPlayer][0][indiceCheval] = 0;
               // gameBoard[nextPlayer][0][indiceCheval + dice] = 1;
            }else if(horse == 1){
                int indiceCheval = recherche(1,gameBoard,nextPlayer,1,lengthOfGb);
                //before moving the horse, check if there is a horse on the case where the horse is going to move
                int checkIfEmpty =isCaseEmpty(nextPlayer,1,indiceCheval+dice,gameBoard,n);
                if(checkIfEmpty==-1){
                    changeCase(nextPlayer,1,indiceCheval,gameBoard, 0);
                    changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);
                }else{
                    //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                    changeCaseToZero(checkIfEmpty,gameBoard);
                    changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);
                }

              //  changeCase(nextPlayer,1,indiceCheval,gameBoard, 0);
              //  changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);
                caseMoved = ((indiceCheval + dice) % nextPlayer*2*56);
                printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);
                
               // gameBoard[nextPlayer][1][indiceCheval] = 0;
               // gameBoard[nextPlayer][1][indiceCheval + dice] = 1;
            }
        }

        //if the player doesnt have a horse on the gameboard
        //if the lastplayer to play was n-1, the next player is 0
        if(nextPlayer == n-1){
            nextPlayer = 0;
        }
        //if the lastplayer to play wasnt n-1, the next player is the lastplayer +1
        else{
            nextPlayer++;
        }


    }else{
        diceIsSix = true;
        if(recherche(1,gameBoard,nextPlayer,0,lengthOfGb) == -1 && recherche(1,gameBoard,nextPlayer,1,lengthOfGb) == -1){
            //sort le premier cheval du joueur
            //before moving the horse, check if there is a horse on the case where the horse is going to move
            int checkIfEmpty =isCaseEmpty(nextPlayer,0,0,gameBoard,n);
            if(checkIfEmpty==-1){
                changeCase(nextPlayer,0,0,gameBoard, 1);
            }else{
                //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                changeCaseToZero(checkIfEmpty,gameBoard);
                changeCase(nextPlayer,0,0 ,gameBoard, 1);
            }
            //changeCase(nextPlayer,0,0,gameBoard, 1);
            //gameBoard[nextPlayer][0][0] = 1;
            printf("Joueur %d  a mis un cheval sur le plateau !\n", nextPlayer);
            
        }else{
            if(recherche(1,gameBoard,nextPlayer,1,lengthOfGb) != -1 && recherche(1,gameBoard,nextPlayer,0,lengthOfGb) == -1){
                printf("Joueur %d : Voulez vous sortir un cheval de l'écurie ou avancer votre second cheval ? (1 ou 2)\n", nextPlayer);
                int choice;
                scanf("%d", &choice);
                if(choice == 1){
                    //before moving the horse, check if there is a horse on the case where the horse is going to move
                    int checkIfEmpty =isCaseEmpty(nextPlayer,0,0,gameBoard,n);
                    if(checkIfEmpty==-1){
                        changeCase(nextPlayer,0,0,gameBoard, 1);
                    }else{
                        //if there is a horse on the case where the horse is going to move, move the horse back to the stable and put our horse on the case
                        changeCaseToZero(checkIfEmpty,gameBoard);
                        changeCase(nextPlayer,0,0 ,gameBoard, 1);
                    }

                    //changeCase(nextPlayer,0,0,gameBoard, 1);
                    //gameBoard[nextPlayer][0][0] = 1;
                    printf("Joueur %d  a mis un cheval sur le plateau !\n", nextPlayer);
                }else if(choice == 2){
                    int indiceCheval = recherche(1,gameBoard,nextPlayer,1,lengthOfGb);
                    changeCase(nextPlayer,1,indiceCheval,gameBoard, 0);
                    changeCase(nextPlayer,1,indiceCheval+dice ,gameBoard, 1);
                    //gameBoard[nextPlayer][1][indiceCheval] = 0;
                    //gameBoard[nextPlayer][1][indiceCheval + dice] = 1;
                    caseMoved = ((indiceCheval + dice) % nextPlayer*2*56);
                    printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

                }


            }else if( recherche(1,gameBoard,nextPlayer,0,lengthOfGb) != -1 && recherche(1,gameBoard,nextPlayer,1,lengthOfGb) == -1){
                printf("Joueur %d : Voulez vous sortir un cheval de l'écurie ou avancer votre premier cheval ? (1 ou 2)\n", nextPlayer);
                int choice;
                scanf("%d", &choice);
                if(choice == 1){
                    changeCase(nextPlayer,1,0,gameBoard, 1);
                   // gameBoard[nextPlayer][1][0] = 1;
                    printf("Vous avez sorti votre deuxieme cheval de l'écurie\n");
                }else if(choice == 2){
                    int indicePremierCheval = recherche(1,gameBoard,nextPlayer,0,lengthOfGb);
                    changeCase(nextPlayer,0,indicePremierCheval,gameBoard, 0);
                    changeCase(nextPlayer,0,indicePremierCheval+dice,gameBoard, 1);
                   // gameBoard[nextPlayer][0][indicePremierCheval] = 0;
                    //gameBoard[nextPlayer][0][indicePremierCheval + dice] = 1;
                    caseMoved = ((indicePremierCheval + dice) % nextPlayer*2*56);
                    printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

                }
            }else if (recherche(1,gameBoard,nextPlayer,0,lengthOfGb) != -1 && recherche(1,gameBoard,nextPlayer,1,lengthOfGb) != -1){
                printf("Voulez vous avancer le premier cheval ou le deuxième ? (1 ou 2)\n");
                int choice;
                scanf("%d", &choice);
                if(choice == 1){
                    int indicePremierCheval = recherche(1,gameBoard,nextPlayer,0,lengthOfGb);
                    changeCase(nextPlayer,0,indicePremierCheval,gameBoard, 0);
                    changeCase(nextPlayer,0,indicePremierCheval+dice,gameBoard, 1);
                    caseMoved = ((indicePremierCheval + dice) % nextPlayer*2*56);
                    printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

                  
                  //  gameBoard[nextPlayer][0][indicePremierCheval] = 0;
                  //  gameBoard[nextPlayer][0][indicePremierCheval + dice] = 1;
               //     printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);
                }else if(choice == 2){
                    int indiceDeuxiemeCheval = recherche(1,gameBoard,nextPlayer,1,lengthOfGb);
                    changeCase(nextPlayer,1,indiceDeuxiemeCheval,gameBoard, 0);
                    changeCase(nextPlayer,1,indiceDeuxiemeCheval+dice,gameBoard, 1);

                    caseMoved = ((indiceDeuxiemeCheval + dice) % nextPlayer*2*56);
                 //   printf("Joueur %d : Vous avez avancé le premier cheval à la case %d\n", nextPlayer, caseMoved);

                    //gameBoard[nextPlayer][1][indiceDeuxiemeCheval] = 0;
                    //gameBoard[nextPlayer][1][indiceDeuxiemeCheval + dice] = 1;
                    printf("Joueur %d : Vous avez avancé le deuxieme cheval à la case %d\n",nextPlayer, caseMoved);
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
    if(oui == 50  ){
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
    int d1=Random(6) ;
    return d1;
}
//end of the dice roll functions

int recherche(int x, int *tabl, int player , int horse, int taille){
    int res;
    int j = player;
    int k;
    if (horse == 0){
        k = 0;
    }else if(horse == 1){
        k = 56;
    }
    j*=56*2;
    for (int i =j+k; i< j+k+56; i++){
        if (tabl[i]== x){
            res= i;
            return res;
        }
    }
    return -1;
}

//function to change the value of a case in gameBoard using pointer address of gameboard
void changeCase(int player, int horse, int caseBoard, int *array, int value){
    int i ;
    int j;
    int k;
    
    //loop in the 3d array gameboard and look for the case [player][horse][caseBoard] then replace the value with value
    for(i=0; i<3; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                if(i==player && j==horse && k==caseBoard){
                    array[i*2*56+j*56+k] = value;
                }
            }
        }
    }
}
//function to change value of a case in gameBoard to 0
void changeCaseToZero(int caseBoard, int *array){
    array[caseBoard] = 0;
}
int isCaseEmpty(int player, int horse, int caseBoard, int *array, int nbPlayer){
    
            int test = 70;
            for(int i=0; i<nbPlayer*2; i++){
                int caseToCheck = i*2*56+horse*56+caseBoard+test;
                if (caseToCheck > 447){
                    caseToCheck = caseToCheck % 448;
                }
                if(array[caseToCheck] != 0){
                    return caseToCheck;
                }
                test +=56;
            }
            return -1;
    /*if(player == 0){
        if(horse == 0){
            int test = 70;
            for(int i=0; i<nbPlayer*2; i++){
                if(array[i*2*56+horse*56+caseBoard+test] != 0){
                    return false;
                }
                test +=56;
            }
            return true;
        }else if(horse == 1){
            int test = 70;
            for(int i=0; i<nbPlayer*2; i++){
                int caseToCheck = i*2*56+horse*56+caseBoard+test;
                if (caseToCheck > 447){
                    caseToCheck = caseToCheck % 448;
                }
                if(array[caseToCheck] != 0){
                    return false;
                }
                test +=56;
            }
            return true;
        }
        
    }*/
            //array[i*2*56+j*56+caseBoard+add-1] != 0)
      
    
    
            return true;

    
    
    /*if(array[player*2*56+horse*56+caseBoard+112] == 0){
        return true;
    }else{
        return false;
    }*/
}