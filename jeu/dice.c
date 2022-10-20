//fonctions relative au tirage du dé


int Random(int Max)
{
    return (rand() % Max) + 1;
}
int rollDice()
{
    int d1 = Random(6);
    return d1;
}