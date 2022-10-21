#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
// my own file with function in it
#include "dice.c"
#include "gameboard.c"
#include "stairs.c"

int main(int argc, char *argv[])
{
    // get value of n in argument
    int n = atoi(argv[1]);
    // array of pid
    int pidTab[n];

    // array needed
    int checkEmpty[3];
    int indiceWin[2];
    int isAtEnd[n];
    // variable of the game
    int checkDepassement;
    int nextPlayer = 0;
    int dice = 0;
    bool gameIsOver = false;
    int i, j, k;
    int isThereAWin = -1;
    int lengthGb = 0;

    //================================================================================================
    int gameBoard[n][2][56];
    // initialization of the array and pointer of the game
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 56; k++)
            {
                gameBoard[i][j][k] = 0;
                lengthGb++;
            }
        }
    }
    int(*pGameBoard)[2][56] = gameBoard;

    int stairs[n][7];
    // set value of each case of stairs to 0
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 7; j++)
        {
            stairs[i][j] = 0;
        }
    }
    int(*pStairs)[7] = stairs;
    //================================================================================================

    while (!gameIsOver)
    {

        // create n+1 pipes
        int pipes[n + 1][2];
        for (i = 0; i < n + 1; i++)
        {
            if (pipe(pipes[i]) == -1)
            {
                printf("erreur pipe");
                return 1;
            }
        }

        // create n+1 children
        for (i = 0; i < n; i++)
        {
            pidTab[i] = fork();
            if (pidTab[i] == -1)
            {
                printf("erreur création processus");
                return 2;
            }
            if (pidTab[i] == 0)
            {
                srand(time(NULL) + getpid());

                // child process
                // close all pipes except the one he is using
                int j;
                for (j = 0; j < n + 1; j++)
                {
                    if (j != i)
                    {
                        close(pipes[j][0]);
                    }
                    if (j != i + 1)
                    {
                        close(pipes[j][1]);
                    }
                }
                // read from pipe i
                int x;
                if (read(pipes[i][0], &x, sizeof(int)) < 0)
                {
                    printf("erreur lecture pipe");
                    return 3;
                }
                printf("Child %d read x : %d from pipe %d\n", i, x, i);
                // write to pipe i+1

                if (i == nextPlayer)
                {
                    x = rollDice();
                    printf("\nChild %d rolled dice : %d\n", i, x);
                }
                if (write(pipes[i + 1][1], &x, sizeof(int)) < 0)
                {
                    printf("erreur ecriture pipe");
                    return 4;
                }
                // close all pipes
                close(pipes[i][0]);
                close(pipes[i + 1][1]);

                return 0;
            }
        }
        // main process

        // close all pipes except the first one
        for (j = 0; j < n + 1; j++)
        {
            if (j != n)
            {
                close(pipes[j][0]);
            }
            if (j != 0)
            {
                close(pipes[j][1]);
            }
        }
        dice = 0;
        if (write(pipes[0][1], &dice, sizeof(int)) < 0)
        {
            printf("erreur ecriture pipe");
            return 5;
        }
        if (read(pipes[n][0], &dice, sizeof(int)) < 0)
        {
            printf("erreur lecture pipe");
            return 6;
        }
        printf("Main process read x : %d from pipe %d\n", dice, n);

        //case where the dice is != 6
        if (dice != 6)
        {
            int saveStair = rechercheStair(1, pStairs, nextPlayer);
            if (saveStair != -1)
            {
                if (dice == saveStair + 1)
                {
                    changeCaseStair(0, pStairs, nextPlayer, saveStair);
                    changeCaseStair(1, pStairs, nextPlayer, saveStair + 1);
                }
            }
            else
            {
                // if no horse 0 but horse 1 on the gameboard, horse 1 moves
                if (recherche(1, pGameBoard, nextPlayer, 0, n) == -1)
                {
                    if (recherche(1, pGameBoard, nextPlayer, 1, n) != -1)
                    {
                        int indiceCheval = recherche(1, pGameBoard, nextPlayer, 1, n);
                        if (indiceCheval + dice > 55)
                        {
                            checkDepassement = 55 - ((indiceCheval + dice) - 55);
                        }
                        else
                        {
                            checkDepassement = indiceCheval + dice;
                        }
                        int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n);

                        if (checkIfEmpty == 1)
                        {
                            changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                            changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                        }
                        else
                        {
                            getIndiceNotEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n, checkEmpty);
                            changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);

                            changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);

                            printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                        }
                        printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                    }
                }
                // if no horse 1 but horse 0 on the gameboard, horse 0 move
                else if (recherche(1, pGameBoard, nextPlayer, 1, n) == -1)
                {
                    if (recherche(1, pGameBoard, nextPlayer, 0, n) != -1)
                    {

                        int indiceCheval = recherche(1, pGameBoard, nextPlayer, 0, n);

                        if (indiceCheval + dice > 55)
                        {
                            checkDepassement = 55 - ((indiceCheval + dice) - 55);
                        }
                        else
                        {
                            checkDepassement = indiceCheval + dice;
                        }
                        int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n);

                        if (checkIfEmpty == 1)
                        {
                            changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);

                            changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                        }
                        else
                        {
                            getIndiceNotEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n, checkEmpty);

                            changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                            changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                            printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                        }
                        printf("Player %d has moved his horse 0 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                    }
                }
                // if two horse on the gameboard, ask which one to move
                else if (recherche(1, pGameBoard, nextPlayer, 0, n) != -1)
                {
                    if (recherche(1, pGameBoard, nextPlayer, 1, n) != -1)
                    {
                        printPlayerGameBoard(pGameBoard, nextPlayer, n);
                        printf("Which horse do you want to move ? (0 or 1)\n");
                        int horse;
                        scanf("%d", &horse);
                        if (horse == 0)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 0, n);
                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }
                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d has moved his horse 0 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                        else if (horse == 1)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 1, n);

                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }

                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                    }
                }

                // if the lastplayer to play was n-1, the next player is 0
                if (nextPlayer == n - 1)
                {
                    nextPlayer = 0;
                }
                // if the lastplayer to play wasnt n-1, the next player is the lastplayer +1
                else
                {
                    nextPlayer++;
                }
            }
        }
        else//If the dice is 6
        {
            int saveStair = rechercheStair(1, pStairs, nextPlayer);
            if (saveStair != -1)
            {
                if (dice == saveStair + 1)
                {
                    changeCaseStair(0, pStairs, nextPlayer, saveStair);
                    changeCaseStair(1, pStairs, nextPlayer, saveStair + 1);
                }
            }
            else
            {

                // if no horse on the board and dice = 6, horse 0 is put on the gameboard
                if ((recherche(1, pGameBoard, nextPlayer, 0, n) == -1) && (recherche(1, pGameBoard, nextPlayer, 1, n) == -1))
                {
                    int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, 0, n);
                    if (checkIfEmpty == 1)
                    {
                        changeCase(1, pGameBoard, nextPlayer, 0, 0);
                    }
                    else
                    {
                        getIndiceNotEmpty(pGameBoard, nextPlayer, 0, 0, n, checkEmpty);

                        changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                        changeCase(1, pGameBoard, nextPlayer, 0, 0);
                        printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                    }
                    printf("Player %d has moved his horse 0 from stable to case %d\n", nextPlayer, 0);
                }
                else
                { // if horse 1 is on the gameboard and horse 0 is in the stable
                    if ((recherche(1, pGameBoard, nextPlayer, 1, n) != -1) && (recherche(1, pGameBoard, nextPlayer, 0, n) == -1))
                    {
                        printPlayerGameBoard(pGameBoard, nextPlayer, n);

                        printf("Player %d : Do you want to put horse 0 on the gameboard or move horse 1 ? (0 or 1)\n", nextPlayer);
                        int choice;
                        scanf("%d", &choice);
                        //if the player chose 0 -> put horse 0 on the gameboard
                        if (choice == 0)
                        {
                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, 0, n);
                            if (checkIfEmpty == 1)
                            {
                                changeCase(1, pGameBoard, nextPlayer, 0, 0);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 0, 0, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 0, 0);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d has moved his horse 0 from stable to case %d\n", nextPlayer, 0);
                        }
                        //if the player chose 1 -> move horse 1
                        else if (choice == 1)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 1, n);

                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }

                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }

                            printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                    } // if horse 0 is on the gameboard and horse 1 is in the stable
                    else if ((recherche(1, pGameBoard, nextPlayer, 1, n) == -1) && (recherche(1, pGameBoard, nextPlayer, 0, n) != -1))
                    {
                        printPlayerGameBoard(pGameBoard, nextPlayer, n);

                        printf("Player %d : Do you want to move horse 0 or  put horse 1 on the gameboard  ? (0 or 1)\n", nextPlayer);
                        int choice;
                        scanf("%d", &choice);
                        //if the player chose 1 -> put horse 1 on the gameboard
                        if (choice == 1)
                        {
                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, 0, n);
                            if (checkIfEmpty == 1)
                            {
                                changeCase(1, pGameBoard, nextPlayer, 1, 0);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 0, 0, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 0, 0);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d has moved his horse 0 from stable to case %d\n", nextPlayer, 0);
                        }
                        //if the player chose 0 -> move the horse 0
                        else if (choice == 0)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 0, n);

                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }

                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard\n", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                    } // if horse 0 and horse 1 are on the gameboard
                    else if ((recherche(1, pGameBoard, nextPlayer, 1, n) != -1) && (recherche(1, pGameBoard, nextPlayer, 0, n) != -1))
                    {
                        printPlayerGameBoard(pGameBoard, nextPlayer, n);

                        printf(" Do you want to move horse 0 or horse 1 ? (0 or 1)\n");
                        int choice;
                        scanf("%d", &choice);
                        //if the player chose 0 -> move horse 0
                        if (choice == 0)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 0, n);

                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }

                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 0, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 0, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 0, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d moved horse 0 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                        //if the player chose 1 -> move horse 1
                        else if (choice == 1)
                        {
                            int indiceCheval = recherche(1, pGameBoard, nextPlayer, 1, n);

                            if (indiceCheval + dice > 55)
                            {
                                checkDepassement = 55 - ((indiceCheval + dice) - 55);
                            }
                            else
                            {
                                checkDepassement = indiceCheval + dice;
                            }

                            int checkIfEmpty = isEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n);

                            if (checkIfEmpty == 1)
                            {
                                changeCase(0, pGameBoard, nextPlayer, 1, indiceCheval);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                            }
                            else
                            {
                                getIndiceNotEmpty(pGameBoard, nextPlayer, 1, checkDepassement, n, checkEmpty);

                                changeCase(0, pGameBoard, checkEmpty[0], checkEmpty[1], checkEmpty[2]);
                                changeCase(1, pGameBoard, nextPlayer, 1, checkDepassement);
                                printf("The horse %d of player %d is no longer on the gameboard", checkEmpty[1], checkEmpty[0]);
                            }
                            printf("Player %d moved horse 1 from case %d to case %d\n", nextPlayer, indiceCheval, checkDepassement);
                        }
                    }
                }
            }
        }
        printf("next player is : %d\n", nextPlayer);

        // finishing close all pipes
        close(pipes[0][0]);
        close(pipes[n][1]);

        // wait for all children to finish
        for (i = 0; i < n; i++)
        {
            wait(NULL);
        }

        // checkwinner -> get value of the player at case 56
        int checkWinner = checkEndGameBoard(pGameBoard, n);
        // fill the case of the player at case 56 with a 1 (ex: if player 1 is at case 56, the case 56 of the isAtEnd array will be filled with a 1)
        fillIsAtEnd(pGameBoard, n, isAtEnd);

        isThereAWin = checkWinnerStair(pStairs, n);

        if (checkWinner != -1)
        {
            // indiceWin take the value of the payer at case 56 and his horse
            indiceCheckEndGameBoard(pGameBoard, n, indiceWin);
            // save take the value of the indice of the horse wich is a the case 56
            int save = recherche(1, pGameBoard, indiceWin[0], indiceWin[1], n);
            // If the player has no horse on stair -> put one on it
            if (rechercheStair(1, pStairs, indiceWin[0]) == -1)
            {
                stairs[indiceWin[0]][0] = 1;
            }
        }

        if (isThereAWin != -1)
        {
            gameIsOver = true;
            printf("\nPlayer %d has won the game\n", isThereAWin);
        }
    }

    // print gameboard at the end of the game
    printf("Gameboard : \n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 56; k++)
            {
                printf("%d ", gameBoard[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    // print the final stairs
    printf("\nStairs : \n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 7; j++)
        {
            printf("%d ", stairs[i][j]);
        }
        printf("\n");
    }

    return 0;
}