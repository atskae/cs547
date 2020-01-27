#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

void* test(void* id) {
    printf("Thread with ID=%i created!\n", *(int*)id);
    pthread_exit(NULL);
}

int main() {
    pthread_t ids[NUM_THREADS];
    int failed;
    for(int i=0; i<NUM_THREADS; i++) {
        int id = i;
        failed = pthread_create(&ids[i], NULL, test, &id);
        if(failed) {
            printf("Failed to create thread %i\n", i);
        }
    }    
    pthread_exit(NULL); // why do we need this one?

    return 0;
}
