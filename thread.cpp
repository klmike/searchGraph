//thread.c
#include "thread.hpp"

    //queue functions
void createQ(threadQueue* t){
    t->empty = true;
    t->first = NULL;
    t->last = NULL;
}

void qPush(threadQueue* t,void function (void*),argument arg){
    if(t->first == NULL){
        t->first = (threadNode*)malloc(sizeof(threadNode));
        t->first->current = (task*)malloc(sizeof(task));
        t->first->current->arg.id = arg.id;
        t->first->current->arg.mutex = arg.mutex;
        t->first->current->arg.node1 = arg.node1;
        t->first->current->arg.node2 = arg.node2;
        t->first->current->function = function;
        t->first->current->arg.db = arg.db;
        t->first->current->arg.results = arg.results;
        t->first->current->arg.resSize = arg.resSize;
        t->first->next= NULL;
        t->last = t->first;
        t->empty = false;
    }else{
        t->last->next = (threadNode*)malloc(sizeof(threadNode));
        t->last->next->current = (task*)malloc(sizeof(task));
        t->last->next->current->arg.id = arg.id;
        t->last->next->current->arg.mutex = arg.mutex;
        t->last->next->current->arg.node1 = arg.node1;
        t->last->next->current->arg.node2 = arg.node2;
        t->last->next->current->arg.results = arg.results;
        t->last->next->current->arg.resSize = arg.resSize;
        t->last->next->current->function = function;
        t->last->next->current->arg.db = arg.db;
        t->last->next->next = NULL;
        t->last = t->last->next;
    }
}

task* qPull(threadQueue* t){
    task* ret;
    if(!t->empty){
        threadNode* tmp = t->first;
        ret = tmp->current;
        t->first = t->first->next;
        if(t->first == NULL)
            t->empty = true;
        //free(tmp);
    }else{
        ret = NULL;
    }
    return ret;
}


    //job scheduler Functions
jobScheduler *createPool(int thread_count){
    jobScheduler *pool;
    int i;

    pool = (jobScheduler *)malloc(sizeof(jobScheduler));


    /* Initialize */
    pool->count = 0;
    pool->thread_count = 0;
    pool->workerIds = 0;
    pool->running = 0;
    pool->end = false;
    pool->queue = &pool->myQueue;
    createQ(pool->queue);


    /* Allocate thread */
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);

    /* Initialize mutex and conditional variable first */
    if((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
       (pthread_cond_init(&(pool->notify), NULL) != 0) ||
       (pool->threads == NULL) ||
       (pool->queue == NULL)) {
        return NULL;
    }

    if((pthread_mutex_init(&(pool->lockM), NULL) != 0) ||
       (pthread_cond_init(&(pool->notifyM), NULL) != 0) ||
       (pool->threads == NULL) ||
       (pool->queue == NULL)) {
        return NULL;
    }


    /* Start worker threads */
    for(i = 0; i < thread_count; i++) {
        if(pthread_create(&(pool->threads[i]), NULL, createThread , (void*)pool) != 0) {
            //jobScheduler_destroy(pool, 0);
            return NULL;
        }
        pool->thread_count++;
    }
    return pool;
}


void *createThread(void *taskManager){
    jobScheduler *pool = (jobScheduler *)taskManager;
    int threadId = pool->workerIds;
    pthread_mutex_lock(&(pool->lock));
    pool->workerIds++;
    pthread_mutex_unlock(&(pool->lock));
    task* currentTask;
    bool end = false;
    while(!end){
        pthread_mutex_lock(&(pool->lock));

        while((pool->count == 0) && (!pool->end)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }
        if(pool->end == true){
            end = true;
            pthread_mutex_unlock(&(pool->lock));
        }else{
            currentTask = qPull(pool->queue);
            pool->running++;
            pool->count--;
            pthread_mutex_unlock(&(pool->lock));

            currentTask->arg.worker = threadId;
            (*(currentTask->function))(&currentTask->arg);

            pthread_mutex_lock(&(pool->lock));
            pool->running--;
            pthread_mutex_unlock(&(pool->lock));

            pthread_mutex_lock(&(pool->lockM));
            pthread_cond_broadcast(&(pool->notifyM));
            pthread_mutex_unlock(&(pool->lockM));
        }
    }
    pthread_mutex_lock(&(pool->lock));
    pool->thread_count--;
    pthread_mutex_unlock(&(pool->lock));
    return(NULL);
}


int jobSchedulerAdd(jobScheduler *pool, void (*function)(void *),void *arg){
    int ret = 0;
    argument* args = (argument*)arg;


    pthread_mutex_lock(&(pool->lock));

    if(!pool->end){
        qPush(pool->queue,function,*args);
        pool->count++;
    }else{
        ret = -1;
    }
    pthread_mutex_unlock(&pool->lock);

    return ret;
}

int jobSchedulerDestroy(jobScheduler *pool){
    int i, err = 0;

    if(pool == NULL) {

        return -1;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0) {
        return -1;
    }

    do {
        if(pool->end) {
            break;
        }

        pool->end = true;
        if((pthread_cond_broadcast(&(pool->notify)) != 0) ||
           (pthread_mutex_unlock(&(pool->lock)) != 0)) {
            break;
        }

        for(i = 0; i < pool->thread_count; i++) {
            pthread_join(pool->threads[i], NULL);
        }
    } while(0);

    if(!err) {
        jobSchedulerFree(pool);
    }
    return err;
}

int jobSchedulerFree(jobScheduler *pool){
    if(pool == NULL || pool->thread_count > 0) {
        return -1;
    }

    if(pool->threads) {
        free(pool->threads);

        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}

void jobSeperator(void* arg){
    argument* arguments = (argument*)arg;
    int worker = arguments->worker;
    uint32_t one = arguments->node1;
    uint32_t two = arguments->node2;
    uint32_t id = arguments->id;
    uint32_t version = arguments->version;
    database* d = arguments->db;
    uint32_t res = bidBfs(d->toIndex,d->fromIndex,d->toBuffer,d->fromBuffer,one,two,d->gr,d->cnt,d->toTail[worker],d->fromTail[worker],version);
    d->results[id] = res;
}
