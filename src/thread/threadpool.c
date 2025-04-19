#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


// Worker thread function
static void* worker_thread(void* arg) {
    ThreadPool* pool = (struct ThreadPool*)arg;

    while (true) {
        pthread_mutex_lock(&pool->lock);

        // Wait for tasks to be available or for the pool to stop
        while (!pool->task_queue_head && !pool->stop) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if (pool->stop && !pool->task_queue_head) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // Get the next task from the queue
        Task* task = pool->task_queue_head;
        pool->task_queue_head = task->next;
        if (!pool->task_queue_head) {
            pool->task_queue_tail = NULL;
        }

        pthread_mutex_unlock(&pool->lock);

        // Execute the task
        task->func(task->arg);
        free(task);
    }

    return NULL;
}

// Create a thread pool
ThreadPool* threadpool_create(int thread_count, int max_threads, int queue_size) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool) {
        perror("Failed to allocate ThreadPool");
        return NULL;
    }

    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
    if (!pool->threads) {
        perror("Failed to allocate threads");
        free(pool);
        return NULL;
    }

    pool->task_queue_head = NULL;
    pool->task_queue_tail = NULL;
    pool->thread_count = thread_count;
    pool->stop = false;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }

    return pool;
}

// Destroy the thread pool
void threadpool_destroy(ThreadPool* pool) {
    if (!pool) return;

    pthread_mutex_lock(&pool->lock);
    pool->stop = true;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);

    // Free remaining tasks in the queue
    Task* task = pool->task_queue_head;
    while (task) {
        Task* next = task->next;
        free(task);
        task = next;
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}

// Submit a task to the thread pool
int threadpool_submit(TaskFunc func, void* arg, ThreadPool* pool) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (!task) {
        perror("Failed to allocate task");
        return -1;
    }

    task->func = func;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->lock);

    if (pool->task_queue_tail) {
        pool->task_queue_tail->next = task;
    } else {
        pool->task_queue_head = task;
    }
    pool->task_queue_tail = task;

    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    return 0;
}

// Wait for all tasks to complete
void threadpool_wait(ThreadPool* pool) {
    pthread_mutex_lock(&pool->lock);
    while (pool->task_queue_head) {
        pthread_cond_wait(&pool->cond, &pool->lock);
    }
    pthread_mutex_unlock(&pool->lock);
}