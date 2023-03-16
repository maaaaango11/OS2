#include "stdlib.h"
#include <stdio.h>
#include <pthread.h>

#define SUCCESSFUL_RESULT 0
#define EXIT_SUCCESS 0
#define STRINGS_NUMBER 10

void *print_strings() {
    for (int i = 0; i < STRINGS_NUMBER; i++) {
        fprintf(stdout,"new string inside thread\n");
    }
    return NULL;
}

int main() {
    pthread_t thread;
    int pthread_create_result = pthread_create(&thread, NULL, reinterpret_cast<void *(*)(void *)>(print_strings), NULL);
    if (pthread_create_result != SUCCESSFUL_RESULT) {
        perror("error in pthread_create");
        pthread_exit(NULL);
        return EXIT_SUCCESS;
    }
    for (int i = 0; i < 10; i++) {
        fprintf(stdout, "new string in main thread\n");
    }
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
