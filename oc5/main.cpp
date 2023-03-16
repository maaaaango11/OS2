#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define STATUS_SUCCESS 0
#define SLEEP_TIME 2
#define CLEANUP_POP_ARG 0

void cleaner(void *arg) {
    fprintf(stdout, "cleaner was called: %s\n", (char *) arg);
}

void *print_strings() {
    pthread_cleanup_push(cleaner, (void*)"child thread start");
    for (int i = 1; i <= 10; ++i) {
        pthread_testcancel();
        printf("string %d (child)\n", i);
        sleep(1);
    }

    pthread_cleanup_pop(CLEANUP_POP_ARG);

    pthread_exit(NULL);
}


int main() {
    pthread_t pthread_id;
    long long pthread_create_result = pthread_create(&pthread_id, NULL,
                                                     reinterpret_cast<void *(*)(void *)>(print_strings), NULL);
    if (pthread_create_result != STATUS_SUCCESS) {

        pthread_exit(NULL);
    }

    sleep(SLEEP_TIME);

    int pthread_cancel_result = pthread_cancel(pthread_id);
    if (pthread_cancel_result != STATUS_SUCCESS) {

        pthread_exit(NULL);
    }

    pthread_exit(EXIT_SUCCESS);
}

//5.1: как работают cleanup handlers
//5.1.2: почему они работают, хотя если просунуть принт перед _pop, то он не срабоатет
//5.2: cancel как syscall (что бы это ни значило лол)