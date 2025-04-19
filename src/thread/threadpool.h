#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>
#include "threadpool_model.h"


// Function prototypes
ThreadPool* threadpool_create(int min_threads, int max_threads, int queue_size);
void threadpool_destroy(ThreadPool* pool);
int threadpool_submit(TaskFunc func, void* arg, ThreadPool* pool);
void threadpool_wait(ThreadPool* pool);

#endif // THREADPOOL_H
