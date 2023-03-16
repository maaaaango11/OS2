#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define STATUS_SUCCESS 0
#define INTERVAL 2100
#define THREAD_NUM 100
#define MAX_LEN 100
typedef struct input{
    char string[MAX_LEN];
    int fdesc;
} input;


void sleepsort(void* arg){
    input* a = static_cast<input *>(arg);
    int count = -1;
    char* c;
    while(count < 0){
        count = read(a->fdesc, &c, 1); //gets
    }
    const char *str = a->string;
    int len = strlen(str);
    usleep(INTERVAL*len);
    puts(str);
    pthread_exit(EXIT_SUCCESS);
}


int main() {
    FILE* file = fopen("texts.txt", "r");
    int pipefd[2];
    pipe(pipefd);
    //char strings[THREAD_NUM][MAX_LEN];
    input input1[THREAD_NUM];
    pthread_t pthread_id_array[THREAD_NUM];
    int pthread_create_result;
    int num_read = 0;
    while(fgets(input1[num_read].string, MAX_LEN, file) != NULL){
        input1[num_read].fdesc = pipefd[0];
        ++num_read;
    }
    for (int i = 0; i < num_read; i++) {
        pthread_create_result = pthread_create(&pthread_id_array[i], NULL, reinterpret_cast<void *(*)(void *)>(sleepsort), (void *) &input1[i]);

        if (pthread_create_result != STATUS_SUCCESS) {
            pthread_exit(NULL);
        }
    }
    for(int j = 0; j<num_read; j++){
        write (pipe[1],"a", 1);
    }
    close(pipe[1]);
    for (int i = 0; i < num_read; i++) {
        int pthread_join_result = pthread_join(pthread_id_array[i], NULL);
        if (pthread_join_result != STATUS_SUCCESS) {
            perror("error in pthread_join");
            pthread_exit(NULL);
            return EXIT_SUCCESS;
        }
    }
    close(pipe[0]);
    pthread_exit(EXIT_SUCCESS);
}


//delayed start of thread (like barrier) so all start ~at same time
//do pipe >>>>> while(read()) in childs >>>>>>write(str[num_childs]) in main