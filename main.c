/*
    Created by Richard Lynch 

*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

pthread_mutex_t lock;

int main(int argc, char *argv[])
{
// init mutex lock
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nlock mutex init failed\n");
        return 1;
    }

// read args
    int firstarg;
    if (argc < 2 || (firstarg = atoi(argv[1])) <= 0)
    { //convert the first arg to an int
        //if there is not arg entered
        printf("No Args Entered");
        exit(1);
    }

// start clock
    printf("Starting Clock\n");
    clock_t start = clock();


// stop the cock
    printf("Stoping Clock\n");
    clock_t tdiff = clock() - start;

// destroy mutex
    pthread_mutex_destroy(&lock);

// done
    printf("Goodbye\n\n");

    return 0;
}