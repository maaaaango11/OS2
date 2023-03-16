#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define STATUS_SUCCESS 0
#define STRINGS_NUMBER 1000000
#define SLEEP_TIME 1

void *print_strings() {
    for (int i = 0; i < STRINGS_NUMBER; i++) {
        pthread_testcancel();
        printf("new string inside thread\n");
    }
    return NULL;
}

int main() {
    //async
    pthread_t pthread_id;
    void *thread_res;
    int pthread_create_result = pthread_create(&pthread_id, NULL,
                                                     reinterpret_cast<void *(*)(void *)>(print_strings), NULL);
    if(pthread_create_result != STATUS_SUCCESS){
        pthread_exit(NULL);
    }

    sleep(SLEEP_TIME);

    int pthread_cancel_result = pthread_cancel(pthread_id);
    if(pthread_cancel_result != STATUS_SUCCESS){
        pthread_exit(NULL);
    }
    int join_result =  pthread_join(pthread_id, &thread_res);
    if(join_result !=0){
        pthread_exit(NULL);
    }
    if (thread_res == PTHREAD_CANCELED) {
        fprintf(stdout,"thread was cancelled\n");
    } else {
        fprintf(stdout,"Thread terminated normally");
    }
    pthread_exit(EXIT_SUCCESS);
}

//4.1: как работает pthread_cancel изнутри
//4.2: как работают Cancellation Points и как понять что функция содержит в себе cancellation point
//4.3 типы cancel (см. pthread_setcanceltype)
//