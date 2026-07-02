#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#define THREADS 8
#define QUEUE_SIZE 100

typedef struct {
    void (*n)(void* arg);
    void* arg;
} task_t;

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t threads[THREADS];
  task_t tasks_queue[QUEUE_SIZE];
  int queued;
  size_t queue_front;
  size_t queue_back;
  short stop;
} threadpool_t;

void threadpool_init(threadpool_t *pool);
void threadpool_destroy(threadpool_t* pool);
void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg);

#endif
