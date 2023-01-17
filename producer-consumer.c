#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 5
#define PRODUCERS_SIZE 3
#define CONSUMERS_SIZE 3

void print_buffer(int* buffer);
void *producer(void *p);
void *consumer(void *c);

static pthread_mutex_t mutex;
static sem_t empty, full;
static int index_add = 0;
static int index_remove = 0;
static int buffer[BUFFER_SIZE];

void *producer(void *p){
    int item;
    for(size_t i = 0; i < BUFFER_SIZE; ++i){
        item = 1 + rand() % 100;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        // LOCK
        buffer[index_add] = item;
        printf("Producer %d: insert item %d at %d place\n", *((int *)p), buffer[index_add], index_add + 1);
        print_buffer(buffer);
        index_add = (index_add+1) % BUFFER_SIZE;
        // UNLOCK
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *c){
    for(size_t i = 0; i < BUFFER_SIZE; ++i){
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        // LOCK
        int item = buffer[index_remove];
        buffer[index_remove] = 0;
        printf("Consumer %d: remove item %d from %d place\n",*((int *)c), item, index_remove+1);
        print_buffer(buffer);
        index_remove = (index_remove+1) % BUFFER_SIZE;
        // UNLOCK
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

void print_buffer(int* buffer){
    printf("Current buffer: ");
    for(size_t i = 0; i < BUFFER_SIZE; ++i){
        printf("%d ", buffer[i]);
    }
    printf("\n----------------------------\n");
}

int main(){
    pthread_t producers[PRODUCERS_SIZE], consumers[CONSUMERS_SIZE];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);

    int names_producers[PRODUCERS_SIZE]; // naming producers
    int names_consumers[CONSUMERS_SIZE]; // naming consumers
    for(size_t i = 0; i < PRODUCERS_SIZE; ++i){
        names_producers[i] = i + 1;
    }
    for(size_t i = 0; i < CONSUMERS_SIZE; ++i){
        names_consumers[i] = i + 1;
    }

    for(size_t i = 0; i < PRODUCERS_SIZE; ++i) {
        pthread_create(&producers[i], NULL, (void *)producer, (void *)&names_producers[i]);
    }
    for(size_t i = 0; i < CONSUMERS_SIZE; ++i) {
        pthread_create(&consumers[i], NULL, (void *)consumer, (void *)&names_consumers[i]);
    }

    for(size_t i = 0; i < PRODUCERS_SIZE; ++i) {
        pthread_join(producers[i], NULL);
    }
    for(size_t i = 0; i < CONSUMERS_SIZE; ++i) {
        pthread_join(consumers[i], NULL);
    }

    // to avoid memory leak
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}