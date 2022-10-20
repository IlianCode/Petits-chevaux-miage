
//function that need to use the gameboard and update it, search in it etc...

// if there is a horse in [player][horse] array, return his indice if yes, else return -1
int recherche(int x, int (*pGameBoard)[2][56], int player, int horse, int nbPlayer)
{
    int i, j, k;
    int look = x;
    i = player;
    j = horse;
    for (k = 0; k < 56; k++)
    {
        // for each case of the gameboard we check if there is the value = look -> if there is a horse
        if (*(*(*(pGameBoard + i) + j) + k) == look)
        {
            return k;
        }
    }

    // if no horse found, return -1
    return -1;
}

// change the case of the horse in [player][horse][caseToChange] array to x -> used to make the horse move in the gameBoard
void changeCase(int x, int (*pGameBoard)[2][56], int player, int horse, int caseToChange)
{
    // the function take the int to put in the array, the array, and the position in the array( player, horse, caseToChange) in parameters
    int i, j, k;
    i = player;
    j = horse;
    k = caseToChange;
    // the next line mean -> in the array case pGameBoard[i][j][k] put the value x
    *(*(*(pGameBoard + i) + j) + k) = x;
}

// isEmpty function, check if the case is empty for the other players
int isEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer)
{
    int i, j, k;
    i = player;
    j = horse;
    k = caseToCheck;
    // for each player at the same place in the gameboard check if there is a horse
    // check if the second horse of the player is here = no change, check if the first horse of next player is here k=+14
    for (int y = 0; y < nbPlayer; y++)
    {
        for (int x = 0; x < 2; x++)
        {
            if (*(*(*(pGameBoard + y) + x) + k) != 0)
            {
                // printf("\n case non vide :  player : %d, horse : %d, case : %d\n", y, x, k);
                return 0;
            }

            //  printf("[%d][%d][%d] = %d ", y, x, k, *(*(*(pGameBoard +y) + x) +k));
            if (x == 1)
            {
                k += 14;
            }
            // if k > size of array (56) > k-=56 (go back to the beginning of the array)
            if (k > 55)
            {
                k -= 56;
            }
        }
    }
    return 1;
}

int getIndiceNotEmpty(int (*pGameBoard)[2][56], int player, int horse, int caseToCheck, int nbPlayer, int *checkEmpty)
{
    int i, j, k;
    i = player;
    j = horse;
    k = caseToCheck;
    for (int y = 0; y < nbPlayer; y++)
    {
        for (int x = 0; x < 2; x++)
        {
            if (*(*(*(pGameBoard + y) + x) + k) != 0)
            {
                // printf("\n case non vide :  player : %d, horse : %d, case : %d\n", y, x, k);
                checkEmpty[0] = y;
                checkEmpty[1] = x;
                checkEmpty[2] = k;
                return checkEmpty[3];
            }

            //   printf("[%d][%d][%d] = %d ", y, x, k, *(*(*(pGameBoard +y) + x) +k));
            if (x == 1)
            {
                k += 14;
            }
            // if k > size of array (56) > k-=56 (go back to the beginning of the array)
            if (k > 55)
            {
                k -= 56;
            }
        }
    }
    checkEmpty[0] = -1;
    checkEmpty[1] = -1;
    checkEmpty[2] = -1;
    return checkEmpty[3];
}

//print the gameboard of a selected player to see the horses position before playing 
void printPlayerGameBoard(int (*pGameBoard)[2][56], int player, int nbPlayer)
{
    int i, j, k;
    i = player;
    for (j = 0; j < 2; j++)
    {
        printf("Player %d, horse %d :\n", i, j);
        for (k = 0; k < 56; k++)
        {

            printf("%d ", *(*(*(pGameBoard + i) + j) + k));
        }
        printf("\n");
    }
    printf("\n");
}



// function that go trought the gameboard and check for each horse is there is a horse on case [56] -> if yes, the player win
int checkEndGameBoard(int (*pGameBoard)[2][56], int nbPlayer)
{
    int i, j, k;

    for (i = 0; i < nbPlayer; i++)
    {
        for (j = 0; j < 2; j++)
        {
            if (*(*(*(pGameBoard + i) + j) + 55) == 1)
            {
                return i;
            }
        }
    }
    // if no horse found, return -1
    return -1;
}


int indiceCheckEndGameBoard(int (*pGameBoard)[2][56], int nbPlayer,  int *indiceWin)
{
    int i, j, k;

    for (i = 0; i < nbPlayer; i++)
    {
        for (j = 0; j < 2; j++)
        {
            if (*(*(*(pGameBoard + i) + j) + 55) == 1)
            {
                indiceWin[0] = i;
                indiceWin[1] = j;
                return indiceWin[2];
            }
        }
    }
    // if no horse found, return -1
    return -1;
}