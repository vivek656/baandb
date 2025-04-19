#ifndef THREADPOOL_MODEL_H
#define THREADPOOL_MODEL_H


#include <pthread.h>
#include <stdbool.h>

typedef void (*TaskFunc)(void*);

// Task structure
typedef struct Task {
    TaskFunc func;
    void* arg;
    struct Task* next;
} Task;

// ThreadPool structure
typedef struct ThreadPool {
    pthread_t* threads;          // Array of worker threads
    Task* task_queue_head;       // Head of the task queue
    Task* task_queue_tail;       // Tail of the task queue
    int thread_count;            // Number of threads in the pool
    bool stop;                   // Flag to indicate if the pool should stop
    pthread_mutex_t lock;        // Mutex for synchronizing access to the pool
    pthread_cond_t cond;         // Condition variable for task availability
} ThreadPool ;

#endif // THREADPOOL_MODEL_H