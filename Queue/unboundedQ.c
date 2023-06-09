#include "unboundedQ.h"

/******************
* Function Name: init_unbounded
* Input: ---
* Output: UQ*
* Function Operation: init unbounded queue on the heap
******************/
UQ* init_unbounded(){
    UQ* queue = (UQ*) malloc(sizeof(UQ));
    if (queue == NULL) {
        printf("Memory reallocation failed.\n");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;

    //init the full semaphore
    if(sem_init(&queue->full, 0,0) != 0){
        perror("Error initialize 'full'");
        free(queue);
        exit(1);
    }
    //init the mutex
    if(pthread_mutex_init(&queue->mutex, NULL) != 0){
        perror("Error initialize 'mutex'");
        sem_destroy(&queue->full);
        free(queue);
        exit(1);
    }
    return queue;
}
void enqueue_unbounded(UQ* queue, Article* article){

    Node* item = createNode(article);

    //lock the mutex (prevent insertion and takeout at the same time)
    pthread_mutex_lock(&queue->mutex);
    if(queue->head == NULL){
        queue->head = item;
        queue->tail = item;

    }
    else{
        Node* temp = queue->tail;
        queue->tail = item;
        temp->next = item;

    }
    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->full);

}

/******************
* Function Name: dequeue_unbounded
* Input: UQ* queue
* Output:  Article*
* Function Operation: remove and return the first Node's Article from the head of the queue (fifo)
******************/
Article * dequeue_unbounded(UQ* queue){

    sem_wait(&queue->full);
    //lock the mutex (prevent insertion and takeout at the same time)
    pthread_mutex_lock(&queue->mutex);

    Node* tempNode = queue->head;
    //update the queue (critical code)
    queue->head = queue->head->next;

    pthread_mutex_unlock(&queue->mutex);

    return tempNode->article;
}