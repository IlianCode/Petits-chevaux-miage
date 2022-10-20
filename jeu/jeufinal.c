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
int getIndiceNotEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer, int * checkEmpty);
//================================================================================================

int main(int argc, char* argv[]){
    //get value of n in argument
    int n = atoi(argv[1]);
    //array of pid
    int pidTab[n];

    int checkEmpty[3];
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

        if(dice!=6){
            //if no horse 0 but horse 1 on the gameboard, horse 1 moves
            if(recherche(1, pGameBoard,nextPlayer,0,n) ==-1){
                if(recherche(1, pGameBoard,nextPlayer,1,n) !=-1){
                    int indiceCheval = recherche(1, pGameBoard,nextPlayer,1,n);
                    int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, indiceCheval+dice, n);

                    if(checkIfEmpty != -1){
                        changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                        changeCase(1, pGameBoard, nextPlayer, 1, indiceCheval+dice);
                    }
                    else{
                        getIndiceNotEmpty(pGameBoard, nextPlayer, 1, indiceCheval+dice, n, checkEmpty);

                        changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                        changeCase(1, pGameBoard, nextPlayer, 1, indiceCheval+dice);
                        printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);

                    }
                    printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, indiceCheval+dice);
                }
            }
            //if no horse 1 but horse 0 on the gameboard, horse 0 move

            else if(recherche(1, pGameBoard,nextPlayer,0,n) != -1){
                if(recherche(1, pGameBoard,nextPlayer,1,n) ==-1){

                    int indiceCheval = recherche(1,pGameBoard,nextPlayer,0,n);
                    int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, indiceCheval+dice, n);

                    if(checkIfEmpty != -1){
                        changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);
                        changeCase(1, pGameBoard, nextPlayer, 0, indiceCheval+dice);
                    }
                    else{
                        getIndiceNotEmpty(pGameBoard, nextPlayer, 0, indiceCheval+dice, n, checkEmpty);

                        changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                        changeCase(1, pGameBoard, nextPlayer, 0, indiceCheval+dice);
                        printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                    }
                    caseMoved = indiceCheval+dice;
                    printf("Player %d has moved his horse 0 from case %d to case %d\n", nextPlayer, indiceCheval, indiceCheval+dice);
                }
            }
            //if two horse on the gameboard, ask which one to move 
            else if((recherche(1, pGameBoard,nextPlayer,0,n) != -1) && (recherche(1, pGameBoard,nextPlayer,1,n) != -1)){
                printf("Which horse do you want to move ? (0 or 1)\n");
                int horse ;
                scanf("%d", &horse);
                if(horse ==0){
                    int indiceCheval = recherche(1,pGameBoard,nextPlayer,0,n);
                    int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, indiceCheval+dice, n);

                    if(checkIfEmpty != -1){
                        changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);
                        changeCase(1, pGameBoard, nextPlayer, 0, indiceCheval+dice);
                    }
                    else{
                        getIndiceNotEmpty(pGameBoard, nextPlayer, 0, indiceCheval+dice, n, checkEmpty);

                        changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                        changeCase(1, pGameBoard, nextPlayer, 0, indiceCheval+dice);
                        printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                    }
                    caseMoved = indiceCheval+dice;
                    printf("Player %d has moved his horse 0 from case %d to case %d\n", nextPlayer, indiceCheval, indiceCheval+dice);

                }else if(horse == 1){
                    int indiceCheval = recherche(1, pGameBoard,nextPlayer,1,n);
                    int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, indiceCheval+dice, n);

                    if(checkIfEmpty != -1){
                        changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                        changeCase(1, pGameBoard, nextPlayer, 1, indiceCheval+dice);
                    }
                    else{
                        getIndiceNotEmpty(pGameBoard, nextPlayer, 1, indiceCheval+dice, n, checkEmpty);

                        changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                        changeCase(1, pGameBoard, nextPlayer, 1, indiceCheval+dice);
                        printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);

                    }
                    caseMoved = indiceCheval+dice;
                    printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, caseMoved);
                }
                }
            }

            //if the lastplayer to play was n-1, the next player is 0
            if(nextPlayer == n-1){
                nextPlayer = 0;
            }
            //if the lastplayer to play wasnt n-1, the next player is the lastplayer +1
            else{
                nextPlayer++;
            }

        }else {

        }

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
    changeCase(1, pGameBoard, 1, 0, 47);
    printf("%d ", recherche(1, pGameBoard, 0, 0, n));

    //print f gameboard
    for(i=0; i<n; i++){
        for(j=0; j<2; j++){
            for(k=0; k<56; k++){
                printf("%d ", gameBoard[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    if(!isEmpty(pGameBoard, 0, 1, 33, n)){
        getIndiceNotEmpty(pGameBoard, 0, 1, 33, n, checkEmpty);
        printf("\ncheckEmpty[0] : %d, checkEmpty[1] : %d, checkEmpty[2] : %d", checkEmpty[0], checkEmpty[1], checkEmpty[2]);
    }

    
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


//if there is a horse in [player][horse] array, return his indice if yes, else return -1
int recherche(int x, int (*pGameBoard)[2][56], int player, int horse, int nbPlayer){
    int i ,j ,k;
    int look = x;
    for ( i =0; i<nbPlayer; i++) { 
        for ( j =0; j <2; j++) {
            for ( k =0; k<56; k++){
                    //for each case of the gameboard we check if there is the value = look -> if there is a horse 
                   if(*(*(*(pGameBoard +i) + j) +k) == look){
                          return k;
                   }
                
            }
         }
    } 
    //if no horse found, return -1
    return -1;
}



//change the case of the horse in [player][horse][caseToChange] array to x -> used to make the horse move in the gameBoard
void changeCase(int x, int (*pGameBoard)[2][56], int player, int horse, int caseToChange){
    //the function take the int to put in the array, the array, and the position in the array( player, horse, caseToChange) in parameters
    int i ,j ,k;
    i=player;
    j=horse;
    k=caseToChange;
    //the next line mean -> in the array case pGameBoard[i][j][k] put the value x
     *(*(*(pGameBoard +i) + j) +k) = x;
      
}

//isEmpty function, check if the case is empty for the other players
int isEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer){
    int i ,j ,k;
    i = player;
    j = horse;
    k = caseToCheck;
    //for each player at the same place in the gameboard check if there is a horse
    //check if the second horse of the player is here = no change, check if the first horse of next player is here k=+14 
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
            //if k > size of array (56) > k-=56 (go back to the beginning of the array)
            if(k>55){
                k-=56;

            }   
        }
        
    }
    return 1;
}

int getIndiceNotEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer, int * checkEmpty){
    int i ,j ,k;
    i = player;
    j = horse;
    k = caseToCheck;
    for(int y=0; y<nbPlayer; y++){
        for(int x =0; x<2; x++){
                if(*(*(*(pGameBoard +y) + x) +k) != 0){
                    printf("\n case non vide :  player : %d, horse : %d, case : %d\n", y, x, k);
                    checkEmpty[0] = y;
                    checkEmpty[1] = x;
                    checkEmpty[2] = k;
                    return checkEmpty[3];
                }
            
            printf("[%d][%d][%d] = %d ", y, x, k, *(*(*(pGameBoard +y) + x) +k));
            if (x==1){
                k+=14;
            }
            //if k > size of array (56) > k-=56 (go back to the beginning of the array)
            if(k>55){
                k-=56;

            }   
        }
        
    }
    checkEmpty[0] = -1;
    checkEmpty[1] = -1;
    checkEmpty[2] = -1;
    return checkEmpty[3];

}