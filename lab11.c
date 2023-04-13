#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    double *a48;
    double sum48;
    int N48;
    int size48;
    long tid48;
} threadargument;

void *compute(void *arg) {
    threadargument *args = (threadargument *)arg;
    int myStart48, myEnd48, myN48, i;

    // Extract arguments from the structure
    double *a48 = args->a48;
    double mysum48 = 0.0;
    int N48 = args->N48;
    int size48 = args->size48;
    long tid48 = args->tid48;

    // determine start and end of computation for the current thread
    myN48 = N48 / size48;
    myStart48 = tid48 * myN48;
    myEnd48 = myStart48 + myN48;
    if (tid48 == (size48 - 1))
        myEnd48 = N48;

    // compute partial sum
    for (i = myStart48; i < myEnd48; i++)
        mysum48 += a48[i];

    // grab the lock, update global sum, and release lock
    pthread_mutex_lock(&mutex);
    args->sum48 += mysum48;
    pthread_mutex_unlock(&mutex);

    return (NULL);
}

int main(int argc, char **argv) {
    long i;
    pthread_t *tid48;

    if (argc != 3) {
        printf("Usage: %s <# of elements> <# of threads>\n", argv[0]);
        exit(-1);
    }

    int N48 = atoi(argv[1]); // no. of elements
    int size48 = atoi(argv[2]); // no. of threads

    // Allocate vector and initialize
    double *a48 = (double *)malloc(sizeof(double) * N48);
    for (i = 0; i < N48; i++)
        a48[i] = (double)(i + 1);

    // Create threads
    tid48 = (pthread_t *)malloc(sizeof(pthread_t) * size48);
    threadargument *args = (threadargument *)malloc(sizeof(threadargument) * size48);
    for (i = 0; i < size48; i++) {
        args[i].a48 = a48;
        args[i].sum48 = 0.0;
        args[i].N48 = N48;
        args[i].size48 = size48;
        args[i].tid48 = i;
        pthread_create(&tid48[i], NULL, compute, (void *)&args[i]);
    }

    // Wait for threads to complete
    for (i = 0; i < size48; i++)
        pthread_join(tid48[i], NULL);

    double TOTALS = 0.0;
    for (i = 0; i < size48; i++)
        TOTALS += args[i].sum48;

    printf("The total is %g, it should be equal to %g\n",
           TOTALS, ((double)N48 * (N48 + 1)) / 2);

    // Clean up
    free(a48);
    free(tid48);
    free(args);

    return 0;
}