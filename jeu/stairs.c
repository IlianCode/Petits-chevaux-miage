//function that use the finals stairs array


int rechercheStair(int x, int (*pStairs)[7], int player){
    int i;
    for (i = 0; i < 7; i++)
    {
        if (*(*(pStairs + player) + i)  == x)
        {
            return i;
        }
    }
    return -1;
}


void changeCaseStair(int x, int (*pStairs)[7], int player,int caseToChange)
{
    // the function take the int to put in the array, the array, and the position in the array( player, horse, caseToChange) in parameters
    int i, j, k;
    i = player;
   // j = horse;
    k = caseToChange;
    // the next line mean -> in the array case pGameBoard[i][j][k] put the value x
    *(*(pStairs + i) + k) = x;
}


//check for each player if the 7 case of stairs is full
int checkWinnerStair(int (*pStairs)[7], int nbPlayer){
    int i, j;
    for (i = 0; i < nbPlayer; i++)
    {
        
            if (*(*(pStairs + i) + 6) == 1)
            {
              return i;
            }
            
        
    }
    return -1;
}



