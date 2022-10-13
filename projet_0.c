#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int n;
    // n take the value of the first argument
    n = atoi(argv[1]);
    
    int childPidTable[n];
    int i;

    // create n children processes and store their pid in the table
    for (i = 0; i < n; i++)
    {
        childPidTable[i] = fork();
        if (childPidTable[i] == 0)
        {
            printf("Child %d with pid %d my father is : %d\n" ,i, getpid(), getppid());
            exit(0);
        }
    }
    // wait for all children to finish
    for (i = 0; i < n; i++)
    {
        waitpid(childPidTable[i], NULL, 0);
    }

return 0;
}