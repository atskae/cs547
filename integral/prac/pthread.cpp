#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <pthread.h>

#define NUM_THREADS 5

struct args_t {
    int id;
};

void* test(void* args) {
    printf("Thread with ID=%i created!\n", ((args_t*)args)->id);
    free(args);
    pthread_exit(NULL);
}

int main() {
    pthread_t ids[NUM_THREADS];
    int failed;
    for(int i=0; i<NUM_THREADS; i++) {
        args_t* args = (args_t*) malloc(sizeof(args_t));
        args->id = i;
        failed = pthread_create(&ids[i], NULL, test, (void*) args);
        assert(!failed);
    }

    pthread_exit(NULL);
}
