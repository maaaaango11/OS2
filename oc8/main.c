#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/resource.h>

#define NUM_STEPS 20000
#define SUCCESSFUL_RESULT 0


struct thread_data {
    int index;
    int numOfThreads;
    double piPart;
};

void *calculate_pi_thread(void *param) {
    struct thread_data *data = (struct thread_data *) param;

    int index = data->index;
    int numOfThreads = data->numOfThreads;

    double pi = 0;
    int i;

    for (i = index; i < NUM_STEPS ; i += numOfThreads) {
        pi += 1.0/(i*4.0 + 1.0);
        pi -= 1.0/(i*4.0 + 3.0);
    }
    pi = pi * 4.0;

    data->piPart = pi;
}

int main(int argc, char **argv) {
    struct rlimit lim;
    if (argc != 2){
        fprintf(stdout,"Wrong number of arguments\n");
        return EXIT_FAILURE;
    }

    char *end;
    int numOfThreads = strtol(argv[1], &end, 10);

    if (numOfThreads <= 0){
        fprintf(stdout,"Wrong number of threads\n");
        return EXIT_FAILURE;
    }
    getrlimit(RLIMIT_NPROC, &lim);
    if(numOfThreads > lim.rlim_cur){
        fprintf(stdout,"Not enough resources\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout,"Using %d threads\n", numOfThreads);

    pthread_t threads[numOfThreads];
    struct thread_data threadData[numOfThreads];
    int i;
    for (i = 0; i < numOfThreads; i++){
        threadData[i].index = i;
        threadData[i].numOfThreads = numOfThreads;

        int pthreadCreateResult = pthread_create(&threads[i], NULL, calculate_pi_thread, &threadData[i]);
        if (pthreadCreateResult != SUCCESSFUL_RESULT) {
            int j;
            for (j = 0; j < i; j++){
                pthread_cancel(threads[j]);
            }

            pthread_exit(NULL);
        }
    }

    double sum = 0;
    for (i = 0; i < numOfThreads; i++){
        int pthreadJoinResult = pthread_join(threads[i], NULL);
        if (pthreadJoinResult != SUCCESSFUL_RESULT){

            pthread_exit(NULL);
        }
        sum += threadData[i].piPart;
    }
    printf("pi done - %.15g\n", sum);
    printf("compare - 3.14159265358979\n");

    return EXIT_SUCCESS;
}
