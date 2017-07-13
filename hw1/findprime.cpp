#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[])
{
    int count;
    int first;
    int i;
    int index;
    char *marked;
    long long int n;
    long long int N;
    int prime;
    if (argc != 2)
    {
        printf ("Command line: %s <m>\n", argv[0]); exit (1);
    }
    n = atoi(argv[1]);
    N = n+1;
    marked = (char *) malloc (N); //alocate slots for numbers in range [0,n]
    if (marked == NULL)
    {
        printf ("Cannot allocate enough memory\n");
        exit (1);
    }
    for (i = 0; i < N; i++)
        marked[i] = 1;
    marked[0] = 0;
    marked[1] = 0; // not primes
    index = 2;
    prime = 2;
    do {
        first = 2 * prime;
        for (i = first; i < N; i += prime)
            marked[i] = 0;
        while (!marked[++index]) ;
        prime = index;
    } while (prime * prime <= n);
    count = 0;
    for (i = 0; i < N; i++)
        count += marked[i];
    printf ("\nThere are %d primes less than or equal to %d\n\n", count, n);
    return 0;
}

