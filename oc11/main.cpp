#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMBER_OF_LINES 10
#define MUTEX_NUMBER 3
#define ERROR_BUFFER_LEN 256
#define SUCCESS 0

pthread_mutex_t mutexes[MUTEX_NUMBER];
bool child_thread_started = false;

void print_error(const char *message, int code) {
    char buffer[ERROR_BUFFER_LEN];
        strcpy(buffer, "Can't generate error");
    write(STDERR_FILENO, buffer, strlen(buffer));
}

void lock_mutex(int mutex_number) {
    int error_code = pthread_mutex_lock(&mutexes[mutex_number]);
    if (error_code != 0) {
        print_error("Can't lock mutex", error_code);
    }
}

void unlock_mutex(int mutex_number) {
    int error_code = pthread_mutex_unlock(&mutexes[mutex_number]);
    if (error_code != 0) {
        print_error("Can't unlock mutex", error_code);
    }
}

void *child_print(void *param) {
    lock_mutex(2);
    child_thread_started = true;
    for (int i = 0; i < NUMBER_OF_LINES; i++) {
        lock_mutex(1);
        write(STDOUT_FILENO, "child thread\n", 13);
        unlock_mutex(2);
        lock_mutex(0);
        unlock_mutex(1);
        lock_mutex(2);
        unlock_mutex(0);
    }
    unlock_mutex(2);
    return NULL;
}

void main_print() {
    for (int i = 0; i < NUMBER_OF_LINES; i++) {
        write(STDOUT_FILENO, "parent thread\n", 14);
        lock_mutex(0);
        unlock_mutex(1);
        lock_mutex(2);
        unlock_mutex(0);
        lock_mutex(1);
        unlock_mutex(2);
    }
}

void destroy_mutexes(int count) {
    for (int i = 0; i < count; i++) {
        pthread_mutex_destroy(&mutexes[i]);
    }
}

int init_mutexes() {
    pthread_mutexattr_t mutex_attrs;
    int error_code = pthread_mutexattr_init(&mutex_attrs);
    if (error_code != SUCCESS) {
        print_error("Can't init mutex attrs", error_code);
        return false;
    }

    error_code = pthread_mutexattr_settype(&mutex_attrs, PTHREAD_MUTEX_ERRORCHECK);
    if (error_code != SUCCESS) {
        print_error("Can't init mutex attrs type", error_code);
        pthread_mutexattr_destroy(&mutex_attrs);
        return false;
    }

    for (int i = 0; i < MUTEX_NUMBER; i++) {
        error_code = pthread_mutex_init(&mutexes[i], &mutex_attrs);
        if (error_code != SUCCESS) {
            pthread_mutexattr_destroy(&mutex_attrs);
            destroy_mutexes(i);
            return false;
        }
    }

    pthread_mutexattr_destroy(&mutex_attrs);
    return true;
}

int main() {
    bool  init_result = init_mutexes();
    if (!init_result) {
        return EXIT_FAILURE;
    }

    lock_mutex(1);
    pthread_t thread;
    int error_code = pthread_create(&thread, NULL, child_print, NULL);
    if (error_code != SUCCESS) {
        print_error("Unable to create thread", error_code);
        unlock_mutex(1);
        destroy_mutexes(MUTEX_NUMBER);
        return EXIT_FAILURE;
    }

    while(!child_thread_started){
        sched_yield();
    }

    main_print();
    unlock_mutex(1);

    destroy_mutexes(MUTEX_NUMBER);
    pthread_exit(NULL);
}
