#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 10
#define PRODUCERS_SIZE 3
#define PRODUCERS_SIZE 3

sem_t empty, full;
pthread_mutex_t mutex;
int index_add = 0;
int index_remove = 0;
int buffer[BUFFER_SIZE];

void *producer(void *pno){
    int item;
    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        item = rand();
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        // LOCK ZONE
        buffer[index_add] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno), buffer[index_add], index_add);
        index_add = (index_add+1) % BUFFER_SIZE;
        // UNLOCK ZONE
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer(void *cno){
    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        // LOCK ZONE
        int item = buffer[index_remove];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno), item, index_remove);
        index_remove = (index_remove+1)%BUFFER_SIZE;
        // UNLOCK ZONE
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(){
    pthread_t producers[BUFFER_SIZE], consumers[BUFFER_SIZE];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);

    int names[BUFFER_SIZE] = {1,2,3,4,5}; // for naming the producer and consumer

    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        pthread_create(&producers[i], NULL, (void *)producer, (void *)&names[i]);
    }
    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        pthread_create(&consumers[i], NULL, (void *)consumer, (void *)&names[i]);
    }

    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        pthread_join(producers[i], NULL);
    }
    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
        pthread_join(consumers[i], NULL);
    }

    // to avoid memory leak
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}