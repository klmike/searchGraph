//thread.h
#include "bidbfs.hpp"

using namespace std;

#define MAX_THREADS 64

typedef struct database{
    nodeIndex* toIndex;
    nodeIndex* fromIndex;
    buffer* toBuffer;
    buffer* fromBuffer;
    grail* gr;
    cncted* cnt;
    tail** toTail;
    tail** fromTail;
    uint32_t* results;
    uint32_t resSize;
}database;

typedef struct argument{
    pthread_mutex_t* mutex;
    int worker;
    uint32_t version;
    uint32_t id;
    uint32_t node1;
    uint32_t node2;
    uint32_t* results;
    uint32_t* resSize;
    database* db;
}argument;

typedef struct task{
    void (*function)(void *);
    argument arg;
}task;

typedef struct threadNode{
    task* current;
    struct threadNode* next;
}threadNode;

typedef struct threadQueue{
    threadNode* first;
    threadNode* last;
    bool empty;
}threadQueue;

typedef struct jobScheduler{
    pthread_mutex_t lock;
    pthread_mutex_t lockM;
    pthread_cond_t notify;
    pthread_cond_t notifyM;
    pthread_t *threads;
    int thread_count;
    int workerIds;
    int running;
    threadQueue myQueue;
    threadQueue *queue;
    int count;
    bool end;
}jobScheduler;


    //Queue Functions
void  createQ(threadQueue*);
void  qPush(threadQueue*,argument);
task* qPull(threadQueue*);

    //rest
void testTask(void*);
jobScheduler *createPool(int);
void *createThread(void *jobScheduler);
int jobSchedulerAdd(jobScheduler*pool,void (*routine)(void *),void*);
int jobSchedulerFree(jobScheduler*);
int jobSchedulerDestroy(jobScheduler*);

    //tasks
void jobSeperator(void*);
